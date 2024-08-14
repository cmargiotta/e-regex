#ifndef E_REGEX_NODES_GET_EXPRESSION_HPP_
#define E_REGEX_NODES_GET_EXPRESSION_HPP_

#include <tuple>

#include "utilities/pack_string.hpp"

namespace e_regex::nodes
{
    template<typename... children>
    constexpr auto get_children_expression()
    {
        if constexpr (sizeof...(children) == 0)
        {
            return static_string {""};
        }
        else if constexpr (sizeof...(children) == 1)
        {
            return std::tuple_element_t<0, std::tuple<children...>>::expression;
        }
        else
        {
            return concatenate_pack_strings_t<
                pack_string<'|'>,
                build_pack_string_t<("(?:" + children::expression + ")")>...>::string;
        }
    }
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_GET_EXPRESSION_HPP_*/
