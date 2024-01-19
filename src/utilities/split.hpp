#ifndef UTILITIES_SPLIT
#define UTILITIES_SPLIT

#include <tuple>

#include "balanced.hpp"
#include "reverse.hpp"
#include "static_string.hpp"

namespace e_regex
{
    template<char separator, typename tokens, typename current = std::tuple<std::tuple<>>>
    struct split;

    template<char separator, typename... tail, typename... current_tokens, typename... currents>
        requires balanced_v<tail...> && balanced_v<current_tokens...>
    struct split<separator,
                 std::tuple<pack_string<separator>, tail...>,
                 std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            // Separator found
            using current = std::tuple<std::tuple<>, std::tuple<current_tokens...>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename head, typename... tail, typename... current_tokens, typename... currents>
    struct split<separator, std::tuple<head, tail...>, std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            using current = std::tuple<std::tuple<current_tokens..., head>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename current>
    struct split<separator, std::tuple<>, current>
    {
            using type = reverse_t<current>;
    };

    template<char separator, typename tokens>
    using split_t = typename split<separator, tokens>::type;
}// namespace e_regex

#endif /* UTILITIES_SPLIT */
