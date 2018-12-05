#pragma once

#include "filesystem.hpp"
#include "search.hpp"
#include <ostream>
#include <set>
#include <string>
#include <unordered_set>
#include <sstream>
#include "cereal/archives/json.hpp"

namespace ioutils {
    namespace mlocate {
        struct Policy {
          public:
            using container_type = std::vector<ioutils::Stats>;
            const container_type &get_data() const { return data; }

            void print() const {
                for (auto const &item : data) {
                    fmt::print("{0:b} {1:>10} {2}\n", item.st_mode & MODE_MASK, item.st_size,
                               item.path);
                }
            }

          protected:
            bool is_valid_dir(const char *dname) const {
                return filesystem::is_valid_dir(dname);
            }
            void process_file(const std::string &parent, const char *stem) {
                ioutils::Stats info;
                info.path = parent + "/" + stem;

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

            void process_symlink(const std::string &parent, const char *stem) {
                process_file(parent, stem);
            }

            void process_dir(const std::string) const {}

            container_type data;
            struct stat statbuf;
            static constexpr int MODE_MASK = 0xfff;
        };

    } // namespace mlocate

    template <typename OArchive, typename T> std::string save(T &&data) {
        std::stringstream os;
        {
            OArchive oar(os);
            oar(CEREAL_NVP(data));
        }
        return os.str();
    }

    template <typename IArchive, typename T> T load(std::string &&buffer) {
        std::stringstream is(buffer);
        IArchive iar(is);
        T data;
        iar(data);
        return data;
    }

    template <typename Matcher, typename Params> class LocatePolicy {
      public:
        LocatePolicy(const Params &args)
            : matcher(args.pattern, args.parameters.mode), params(args) {}

        void process(const char *begin, const size_t len) {
            constexpr char EOL = '\n';
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                process_line(start, ptr - start + 1);
                start = ++ptr;
                if (start == end) break;
            }
        }

      private:
        Matcher matcher;
        Params params;
        void process_line(const char *begin, const size_t len) {
            if (matcher.is_matched(begin, len)) {
                fmt::print("{0}{1}", params.parameters.prefix, std::string(begin, len));
            }
        }
    };

    template <typename Params> class PrintAllPolicy {
      public:
        PrintAllPolicy(const Params &args) : params(args) {}
        void process(const char *begin, const size_t len) {
            constexpr char EOL = '\n';
            const char *start = begin;
            const char *end = begin + len;
            const char *ptr = begin;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                process_line(start, ptr - start + 1);
                start = ++ptr;
                if (start == end) break;
            }
        }

      private:
        Params params;
        void process_line(const char *begin, const size_t len) {
            fmt::print("{0}{1}", params.parameters.prefix, std::string(begin, len));
        }
    };
} // namespace ioutils
