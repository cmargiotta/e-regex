#ifndef E_REGEX_OPERATORS_COMMON_HPP_
#define E_REGEX_OPERATORS_COMMON_HPP_

#include <tuple>

#include "nodes/basic.hpp"
#include "tree_builder/common.hpp"

namespace e_regex
{
    template<typename last_node, typename tokens, auto group_index>
    struct tree_builder_helper;

    template<typename last_node, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<>, group_index>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<head, tail...>, group_index>
    {
            // Simple case, iterate
            using new_node = typename tree_builder_helper<
                nodes::simple<terminals::terminal<head>>,
                std::tuple<tail...>,
                group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
} // namespace e_regex

#endif /* E_REGEX_OPERATORS_COMMON_HPP_*/
