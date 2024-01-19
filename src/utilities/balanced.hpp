#ifndef UTILITIES_BALANCED_HPP
#define UTILITIES_BALANCED_HPP

#include "static_string.hpp"

namespace e_regex
{
    template<unsigned round_par, unsigned square_par, unsigned braces, typename... strings>
    struct balanced;

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<'('>, strings...>
        : public balanced<round_par + 1, square_par, braces, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<')'>, strings...>
        : public balanced<round_par - 1, square_par, braces, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<'['>, strings...>
        : public balanced<round_par, square_par + 1, braces, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<']'>, strings...>
        : public balanced<round_par, square_par - 1, braces, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<'{'>, strings...>
        : public balanced<round_par, square_par, braces + 1, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename... strings>
    struct balanced<round_par, square_par, braces, pack_string<'}'>, strings...>
        : public balanced<round_par, square_par, braces - 1, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces, typename head, typename... strings>
    struct balanced<round_par, square_par, braces, head, strings...>
        : public balanced<round_par, square_par, braces, strings...>
    {
    };

    template<auto round_par, auto square_par, auto braces>
    struct balanced<round_par, square_par, braces>
    {
            static constexpr bool value = round_par == 0 && square_par == 0 && braces == 0;
    };

    template<typename... strings>
    static constexpr auto balanced_v = balanced<0, 0, 0, strings...>::value;

}// namespace e_regex

#endif /* UTILITIES_BALANCED_HPP */
