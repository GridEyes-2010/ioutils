#include "fmt/format.h"
#include "search.hpp"
<<<<<<< HEAD
#include "search_policies.hpp"
#include "utilities.hpp"
#include "utils/timeutils.hpp"
=======
>>>>>>> master

int main(int argc, char *argv[]) {
    std::vector<std::string> p;
    for (auto idx = argc - 1; idx > 0; --idx) {
<<<<<<< HEAD
        std::string path(argv[idx]);
        ioutils::remove_trailing_slash(path);
        p.emplace_back(path);
=======
        p.emplace_back(argv[idx]);
        using Search = typename ioutils::FileSearch<ioutils::ConsolePolicy>;
        // using Search = typename ioutils::FileSearch<ioutils::StorePolicy>;
        Search search;
        search.dfs(p);
        // fmt::print("Number of files: {}\n", search.get_files().size());
>>>>>>> master
    }

    using Search = typename ioutils::FileSearch<ioutils::ConsolePolicy>;
    // using Search = typename ioutils::FileSearch<ioutils::StorePolicy>;
    Search search;
    search.dfs(p);
    return EXIT_SUCCESS;
}
