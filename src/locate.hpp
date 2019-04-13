#pragma once

#include "fdwriter.hpp"
#include "filesystem.hpp"
#include "search.hpp"
#include <fcntl.h>
#include <string>
#include <unistd.h>

namespace ioutils {
    namespace locate {
        /**
         * This policy will write the output to the given file.
         */
        struct UpdateDBStreamPolicy {
          public:
            UpdateDBStreamPolicy() : writer(StreamWriter::STDOUT) {}

            template <typename Params>
            UpdateDBStreamPolicy(Params &&params) : writer(params.database.data()) {}

          protected:
            bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
            void process_file(const Path &parent, const char *stem = nullptr) {
                writer.write(parent.path.data(), parent.path.size());
                if (stem != nullptr) {
                    writer.sep();
                    writer.write(stem, strlen(stem));
                }
                writer.eol();
            }

            void process_symlink(const Path &parent, const char *stem = nullptr) {
                process_file(parent, stem);
            }
            void process_fifo(const Path &parent, const char *stem = nullptr) {
                process_file(parent, stem);
            }
            void process_chr(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }
            void process_blk(const Path &parent, const char *stem = nullptr) { process_file(parent, stem); }
            void process_socket(const Path &parent, const char *stem = nullptr) {
                process_file(parent, stem);
            }
            void process_whiteout(const Path &parent, const char *stem = nullptr) {
                process_file(parent, stem);
            }

            void process_dir(const std::string &path) {
                writer.write(path.data(), path.size());
                writer.eol();
            }

          private:
            StreamWriter writer;
        };
    } // namespace locate

    template <typename Matcher> class LocateStreamPolicy {
      public:
        template <typename Params>
        LocateStreamPolicy(Params &&params)
            : matcher(params.pattern, params.regex_mode),
              linebuf(),
              prefix(params.prefix),
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

        // Note: Override this function to make FileReader happy. We do not care about the database name in
        // fast-locate.
        void set_filename(const char *) {}

        // Process text data in the linebuf.
        void finalize() {
            process_line(linebuf.data(), linebuf.size());
            linebuf.clear();
        }
    };

    class PrintAllPolicy {
      public:
        template <typename Params>
        PrintAllPolicy(Params &&args) : prefix(args.prefix), console(StreamWriter::STDOUT) {}
        void process(const char *begin, const size_t len) { console.write(begin, len); }

      protected:
        void set_filename(const char *) {}
        void finalize() {}

      private:
        std::string prefix;
        StreamWriter console;
        // void process_line(const char *begin, const size_t len) { console.write(begin, len); }
    };
} // namespace ioutils