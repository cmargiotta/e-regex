#ifndef E_REGEX
#define E_REGEX

#include "tree_builder.hpp"

namespace e_regex
{
    template<auto regex>
    auto match(auto expression)
    {
        using packed = build_pack_string_t<regex>;
        return tree_builder<packed>::tree::match(expression);
    }
}// namespace e_regex

template<typename T, T... regex>
constexpr auto operator"" _regex()
{
    return typename e_regex::tree_builder<e_regex::pack_string<regex...>>::tree {};
}

#endif /* E_REGEX */
