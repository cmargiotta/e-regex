#ifndef OPERATORS_START_ANCHOR
#define OPERATORS_START_ANCHOR

#include "common.hpp"
#include "operators/basic_node.hpp"
#include "terminals/anchors/start.hpp"

namespace e_regex
{
    template<typename... tail>
    struct tree_builder_helper<void, std::tuple<pack_string<'^'>, tail...>>
    {
            // Start anchor found

            using tree = typename tree_builder_helper<
                basic_node<terminals::anchors::start, std::tuple<>, 1, 1, policy::POSSESSIVE>,
                std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_START_ANCHOR */
