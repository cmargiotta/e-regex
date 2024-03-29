#ifndef OPERATORS_END_ANCHOR_HPP
#define OPERATORS_END_ANCHOR_HPP

#include <tuple>

#include "common.hpp"
#include "heuristics.hpp"
#include "nodes.hpp"
#include "static_string.hpp"
#include "terminals/anchors/end.hpp"

namespace e_regex
{
    template<typename last_node, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'$'>>, group_index>
    {
            // End anchor found

            using new_node = nodes::simple<terminals::anchors::end>;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_END_ANCHOR_HPP */
