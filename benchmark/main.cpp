#include <array>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>

#include "e_regex.hpp"

template<size_t N>
void measure(const std::string& data)
{
    using clock = std::chrono::high_resolution_clock;

    const auto start = clock::now();
    unsigned   count = 0;
    if constexpr (N == 0)
    {
        constexpr e_regex::regex<"[\\w.+\\-]+@[\\w.\\-]+\\.[\\w.\\-]+"> matcher;

        // Email
        for (auto match: matcher(std::string_view {data}))
        {
            count++;
        }
    }
    else if constexpr (N == 1)
    {
        constexpr e_regex::regex<"[\\w]+:\\/\\/[^\\/\\s?#]+[^\\s?#]+(?:\\?[^\\s#]*)?(?:#[^\\s]*)?">
            matcher;
        // URI
        for (auto match: matcher(std::string_view {data}))
        {
            count++;
        }
    }
    else
    {
        constexpr e_regex::regex<"(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9])">
            matcher;
        // IP
        for (auto match: matcher(std::string_view {data}))
        {
            count++;
        }
    }

    const auto   end = clock::now();
    const double elapsed
        = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
              .count()
          * 1e-6;
    std::cout << elapsed << " - " << count << "\n";
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: benchmark <filename>\n";
        return 1;
    }

    std::ifstream file {argv[1]};
    if (!file)
    {
        std::cerr << "unable to open " << argv[1] << "\n";
        return 1;
    }

    const std::string data {std::istreambuf_iterator<char> {file},
                            std::istreambuf_iterator<char> {}};

    measure<0>(data);
    measure<1>(data);
    measure<2>(data);
}
