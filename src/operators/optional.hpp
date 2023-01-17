#ifndef OPERATORS_OPTIONAL
#define OPERATORS_OPTIONAL

#include "common.hpp"
#include "operators/basic_node.hpp"

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, tail...>>
    {
            // ? operator found
            using new_node = set_node_range_t<last_node, 0, 1, policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_OPTIONAL */
