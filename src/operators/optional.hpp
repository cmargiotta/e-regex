#ifndef OPERATORS_OPTIONAL
#define OPERATORS_OPTIONAL

#include <nodes.hpp>

#include "common.hpp"

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, tail...>>
    {
            // greedy ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'?'>, tail...>>
    {
            // lazy ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'+'>, tail...>>
    {
            // possessive ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_OPTIONAL */
