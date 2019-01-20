#include "celero/Celero.h"
#include "fmt/format.h"
#include "ioutils.hpp"
#include <fstream>
#include <iostream>

#include "search.hpp"
#include "search_policies.hpp"
#include "simple_store_policy.hpp"
#include "simple_search_policy.hpp"

constexpr int number_of_samples = 10;
constexpr int number_of_operations = 1;

int test(const std::string &command, const std::string &path) {
    std::string buffer = command + path + " > /tmp/output.log";
    return system(buffer.data());
}

int test_find_regex(const std::string &command, const std::string &regex,
                    const std::string &path) {
    std::string buffer = command + " " + path + " | grep -E " + regex + " > /tmp/output.log";
    return system(buffer.data());
}

int test_mfind_regex(const std::string &command, const std::string &regex,
                     const std::string &path) {
    std::string buffer = command + " -e " + regex + " " + path + " > /tmp/output.log";
    return system(buffer.data());
}

int test_fd_regex(const std::string &command, const std::string &regex,
                  const std::string &path) {
    std::string buffer = command + "." + path + " | grep -E " + regex + "> /tmp/output.log";
    return system(buffer.data());
}

const std::string pattern1{"\'/\\w+options.c(p)*$\'"};

CELERO_MAIN

// Find all files in the boost source code
BASELINE(boost, gnu_find, number_of_samples, number_of_operations) {
    test("find ", "../../3p/src/");
}

BENCHMARK(boost, fd, number_of_samples, number_of_operations) {
    test("fd  . ", "../../3p/src");
}

BENCHMARK(boost, mfind_to_console, number_of_samples, number_of_operations) {
    test("mfind ", "../../3p/src");
}

// Find all files in the kernel source code.
BASELINE(linux_kernels, gnu_find, number_of_samples, number_of_operations) {
    test("find ", "/usr/src/");
}

BENCHMARK(linux_kernels, fd, number_of_samples, number_of_operations) {
    test("fd . ", "/usr/src/");
}

BENCHMARK(linux_kernels, mfind_to_console, number_of_samples, number_of_operations) {
    test("mfind ", "/usr/src/");
}

// Find all files using a regex that does not match any results
BASELINE(boost_regex, gnu_find, number_of_samples, number_of_operations) {
    test_find_regex("find ", pattern1, " ../../3p/src/");
}

BENCHMARK(boost_regex, fd, number_of_samples, number_of_operations) {
    test_fd_regex("fd ", pattern1, " ../../3p/src/");
}

BENCHMARK(boost_regex, mfind_to_console, number_of_samples, number_of_operations) {
    test_mfind_regex("mfind ", pattern1, " ../../3p/src/");
}
