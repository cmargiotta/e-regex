#ifndef UTILITIES_NUMBER_TO_PACK_STRING_HPP
#define UTILITIES_NUMBER_TO_PACK_STRING_HPP

#include <concepts>

#include "static_string.hpp"

namespace e_regex
{
    consteval bool nonzero(auto number)
    {
        return number != 0;
    };

    template<auto number, typename current_string>
        requires std::integral<decltype(number)>
    struct number_to_pack_string;

    template<auto number, char... current>
        requires(!nonzero(number))
    struct number_to_pack_string<number, pack_string<current...>>
    {
            using type = pack_string<current...>;
    };

    template<auto number, char... current>
        requires(nonzero(number))
    struct number_to_pack_string<number, pack_string<current...>>
    {
            static constexpr auto number_10_shifted = number / 10;
            static constexpr auto current_unit      = number - (number_10_shifted * 10);

            using type =
                typename number_to_pack_string<number_10_shifted,
                                               pack_string<(current_unit + '0'), current...>>::type;
    };

    template<>
    struct number_to_pack_string<0, pack_string<>>
    {
            using type = pack_string<'0'>;
    };

    template<auto number>
    using number_to_pack_string_t = typename number_to_pack_string<number, pack_string<>>::type;
}// namespace e_regex

#endif /* UTILITIES_NUMBER_TO_PACK_STRING_HPP */
