#ifndef UTILITIES_REVERSE
#define UTILITIES_REVERSE

#include <tuple>

namespace e_regex
{
    template<typename T, typename current = std::tuple<>>
    struct reverse;

    template<typename T, typename... tail, typename... currents>
    struct reverse<std::tuple<T, tail...>, std::tuple<currents...>>
    {
            using type = typename reverse<std::tuple<tail...>, std::tuple<T, currents...>>::type;
    };

    template<typename... currents>
    struct reverse<std::tuple<>, std::tuple<currents...>>
    {
            using type = std::tuple<currents...>;
    };

    template<typename tuple>
    using reverse_t = typename reverse<tuple>::type;
}// namespace e_regex

#endif /* UTILITIES_REVERSE */
