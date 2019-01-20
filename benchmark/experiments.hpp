#pragma once

#include "fmt/format.h"
#include <string>

namespace ioutils {
    namespace experiments {
        struct LineStatsBase {
            LineStatsBase() : lines(0) {}
            void print() const { fmt::print("Number of lines: {}\n", lines); }

          protected:
            size_t lines = 0;
        };

        template <typename Policy> struct LineStats_std : public Policy {
            LineStats_std() : Policy() {}
            void process(const char *buffer, size_t len) {
                for (size_t idx = 0; idx < len; ++idx) {
                    if (buffer[idx] == EOL) {
                        ++Policy::lines;
                    }
                }
            }
            void finalize() const {}
        };

        template <typename Policy> struct LineStats : public Policy {
            LineStats() : Policy() {}
            void process(const char *buffer, size_t len) {
                const char *end = buffer + len;
                const char *ptr = buffer;
                while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                    ++Policy::lines;
                    ++ptr;
                }
            }
            void finalize() const {}
        };
    } // namespace experiments

} // namespace ioutils
