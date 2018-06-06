#pragma once
#include "fmt/format.h"
#include "unistd.h"
#include <cstring>
#include <limits>

#include "utils/memchr.hpp"

namespace ioutils {
    static constexpr char EOL = '\n';
    namespace experiments {
        struct LineStatsBase {
            LineStatsBase() : lines(0) {}
            void print() const { fmt::print("Number of lines: {}\n", lines); }
            size_t lines = 0;
        };

        template <typename Policy> struct LineStats_std : public Policy {
            explicit LineStats_std() : Policy() {}
            void process(const char *buffer, size_t len) {
                for (size_t idx = 0; idx < len; ++idx) {
                    if (buffer[idx] == EOL) {
                        ++Policy::lines;
                    }
                }
            }
        };

        template <typename Policy>
        struct LineStats : public Policy {
            explicit LineStats() : Policy() {}
            void process(const char *buffer, size_t len) {
                const char *end = buffer + len;
                const char *ptr = buffer;
                while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                    ++Policy::lines;
                    ++ptr;
                }
            }
        };
    } // namespace experiments

    // A simple parser which computes the file size, the number of lines, and
    // the maximum/minimum length of lines.
    class FileStats {
      public:
        void process(const char *begin, const size_t len) {
            // Count the number of lines
            const char *ptr = begin;
            const char *end = ptr + len;
            while ((ptr = static_cast<const char *>(memchr(ptr, EOL, end - ptr)))) {
                // Update the line counter
                ++lines;

                // Update the max and min line length.
                const size_t new_eol = file_size + ptr - begin;
                const size_t len = new_eol - current_eol - 1;
                max_len = len > max_len ? len : max_len;
                min_len = len < min_len ? len : min_len;
                current_eol = new_eol;

                // Move to the next character.
                ++ptr;
            }

            // Update the current position of the read buffer.
            file_size += end - begin;
        }

        void print() const {
            fmt::print("Number of bytes: {}\n", file_size);
            fmt::print("Number of lines: {}\n", lines);
            fmt::print("Max line length: {}\n", max_len);
            fmt::print("Min line lenght: {}\n", min_len);
            fmt::print("File size: {}\n", file_size);
        }

        size_t file_size = 0;
        size_t lines = 0;
        size_t max_len = std::numeric_limits<size_t>::min();
        size_t min_len = std::numeric_limits<size_t>::max();
        size_t current_eol = 0;

      private:
        static constexpr char EOL = '\n';
    };
} // namespace ioutils
