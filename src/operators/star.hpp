#ifndef OPERATORS_STAR
#define OPERATORS_STAR

#include "braces.hpp"
#include "common.hpp"
#include "optional.hpp"

namespace e_regex
{
    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, tail...>, group_index>
    {
            // greedy * operator found
            using new_node = nodes::greedy<last_node, 0>;
            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>, group_index>::tree;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'?'>, tail...>, group_index>
    {
            // lazy * operator found
            using new_node = nodes::lazy<last_node, 0>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>, group_index>::tree;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'+'>, tail...>, group_index>
    {
            // possessive * operator found
            using new_node = nodes::possessive<last_node, 0>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>, group_index>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_STAR */
