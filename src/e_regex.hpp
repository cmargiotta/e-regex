#ifndef E_REGEX
#define E_REGEX

#include "tree_builder.hpp"

namespace e_regex
{

}// namespace e_regex

template<typename T, T... regex>
auto operator"" _regex()
{
    return typename e_regex::tree_builder<e_regex::static_string<regex...>>::tree {};
}

#endif /* E_REGEX */
