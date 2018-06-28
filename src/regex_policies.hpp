#pragma once

#include "filesystem.hpp"
#include "fmt/format.h"
#include <string>

namespace ioutils {
    template <typename Matcher> class RegexPolicy {
      public:
        RegexPolicy(const std::string &pattern) : buffer(), matcher(pattern) {
            buffer.reserve(1023);
        }

      protected:
        bool is_valid_dir(const char *dname) const { return filesystem::is_valid_dir(dname); }
        void process_file(const std::string &parent, const char *stem) {
            buffer = parent + "/" + stem;
            if (matcher.is_matched(buffer.data(), buffer.size())) {
                fmt::print("{}\n", buffer);
            }
        }

        void process_dir(const std::string &p) {
            if (matcher.is_matched(p.data(), p.size())) {
                fmt::print("{}\n", p);
            }
        }

        std::string buffer;
        Matcher matcher;
    };
} // namespace ioutils
