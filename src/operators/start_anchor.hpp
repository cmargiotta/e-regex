#ifndef OPERATORS_START_ANCHOR
#define OPERATORS_START_ANCHOR

#include "common.hpp"
#include "nodes.hpp"
#include "terminals.hpp"

namespace e_regex
{
    template<typename... tail, auto group_index>
    struct tree_builder_helper<void, std::tuple<pack_string<'^'>, tail...>, group_index>
    {
            // Start anchor found

            using tree = typename tree_builder_helper<nodes::simple<terminals::anchors::start>,
                                                      std::tuple<tail...>,
                                                      group_index>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_START_ANCHOR */
