#ifndef E_REGEX_UTILITIES_REVERSE_HPP_
#define E_REGEX_UTILITIES_REVERSE_HPP_

#include <tuple>

#include "concepts.hpp"

namespace e_regex
{
    template<typename T, tuple current = std::tuple<>>
    struct reverse;

    template<typename T, typename... tail, typename... currents>
    struct reverse<std::tuple<T, tail...>, std::tuple<currents...>>
    {
            using type =
                typename reverse<std::tuple<tail...>,
                                 std::tuple<T, currents...>>::type;
    };

    template<typename... currents>
    struct reverse<std::tuple<>, std::tuple<currents...>>
    {
            using type = std::tuple<currents...>;
    };

    template<tuple tuple>
    using reverse_t = typename reverse<tuple>::type;
} // namespace e_regex

#endif /* E_REGEX_UTILITIES_REVERSE_HPP_*/
