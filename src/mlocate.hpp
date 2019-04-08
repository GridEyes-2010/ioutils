#pragma once

#include "fdwriter.hpp"
#include "filesystem.hpp"
#include "search.hpp"
#include <fcntl.h>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unistd.h>

namespace ioutils {
    namespace mlocate {
        struct Policy {
          public:
            using container_type = std::vector<ioutils::Stats>;
            const container_type &get_data() const { return data; }

            void print() const {
                for (auto const &item : data) {
                    fmt::print("{0:b} {1:>10} {2}\n", item.st_mode & MODE_MASK, item.st_size, item.path);
                }
            }

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem) {
                ioutils::Stats info;
                info.path = parent.path + "/" + stem;

                // Skip if we cannot get information of a given file
                if (stat(info.path.data(), &statbuf)) return;

                // Update information
                info.st_mode = statbuf.st_mode;
                info.st_size = statbuf.st_size;
                info.last_access_time = statbuf.st_atime;
                info.modification_time = statbuf.st_mtime;
                info.status_change_time = statbuf.st_ctime;

                // Update data
                data.emplace_back(info);
            }

            void process_file(const Path &parent) {
                ioutils::Stats info;
                info.path = parent.path;

                // Skip if we cannot get information of a given file
                if (stat(info.path.data(), &statbuf)) return;

                // Update information
                info.st_mode = statbuf.st_mode;
                info.st_size = statbuf.st_size;
                info.last_access_time = statbuf.st_atime;
                info.modification_time = statbuf.st_mtime;
                info.status_change_time = statbuf.st_ctime;

                // Update data
                data.emplace_back(info);
            }

            void process_symlink(const Path &parent, const char *stem) { process_file(parent, stem); }

            void process_dir(const std::string) const {}

            container_type data;
            struct stat statbuf;
            static constexpr int MODE_MASK = 0xfff;
        };

        /**
         * This policy will write the output to the given file.
         */
        struct UpdateDBStreamPolicy {
          public:
            UpdateDBStreamPolicy() : writer(StreamWriter::STDOUT) {}

            template <typename Params> UpdateDBStreamPolicy(Params &&params) : writer(params.database.data()) {}

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem) {
                auto path = parent.path + "/" + stem; // TODO: We can do better here.
                writer.write(path.data(), path.size());
                writer.put(EOL);
            }

            void process_file(const Path &parent) {
                writer.write(parent.path.data(), parent.path.size());
                writer.put(EOL);
            }

            void process_symlink(const Path &parent, const char *stem) { process_file(parent, stem); }
            void process_dir(const std::string) const {}

          private:
            StreamWriter writer;
            const char EOL = '\n';
        };
    } // namespace mlocate

    template <typename Matcher> class LocateStreamPolicy {
      public:
        template <typename Params>
        LocateStreamPolicy(Params &&params)
            : matcher(params.pattern, params.regex_mode), linebuf(), prefix(params.prefix),
              console(StreamWriter::STDOUT) {}
        void process(const char *begin, const size_t len) {
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                if (linebuf.empty()) {
                    process_line(start, ptr - start + 1);
                } else {
                    linebuf.append(start, ptr - start + 1);
                    process_line(linebuf.data(), linebuf.size());
                    linebuf.clear();
                }

                // Update parameters
                start = ++ptr;

                // Stop if we reach the end of the buffer.
                if (ptr >= end) break;
            }

            // Update the line buffer with leftover data.
            if (ptr == nullptr) {
                linebuf.append(start, end - start);
            }
        }

      protected:
        Matcher matcher;
        std::string linebuf;
        std::string prefix;
        StreamWriter console;
        static constexpr char EOL = '\n';

        virtual void process_line(const char *begin, const size_t len) {
            if (matcher.is_matched(begin, len)) {
                if (prefix.empty()) {
                    console.write(begin, len);
                } else {
                    console.write(prefix.data(), prefix.size());
                    console.write(begin, len);
                }
            }
        }

        // Note: Override this function to make FileReader happy. We do not care about the database name in mlocate.
        void set_filename(const char *) {}

        // Process text data in the linebuf.
        void finalize() {
            process_line(linebuf.data(), linebuf.size());
            linebuf.clear();
        }
    };

    class PrintAllPolicy {
      public:
        template <typename Params> PrintAllPolicy(Params &&args) : prefix(args.prefix) {}
        void process(const char *begin, const size_t len) {
            constexpr char EOL = '\n';
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                process_line(start, ptr - start + 1);
                start = ++ptr;
            }
        }

      protected:
        void set_filename(const char *) {}
        void finalize() {}

      private:
        std::string prefix;
        void process_line(const char *begin, const size_t len) {
            fmt::print("{0}{1}", prefix, std::string(begin, len));
        }
    };
} // namespace ioutils
