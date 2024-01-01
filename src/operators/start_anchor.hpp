#ifndef OPERATORS_START_ANCHOR
#define OPERATORS_START_ANCHOR

#include <nodes.hpp>
#include <terminals.hpp>

#include "common.hpp"

namespace e_regex
{
    template<typename... tail>
    struct tree_builder_helper<void, std::tuple<pack_string<'^'>, tail...>>
    {
            // Start anchor found

            using tree = typename tree_builder_helper<
                nodes::basic<terminals::anchors::start, std::tuple<>, 1, 1, nodes::policy::POSSESSIVE>,
                std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_START_ANCHOR */
