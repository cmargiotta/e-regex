#ifndef OPERATORS_END_ANCHOR_HPP
#define OPERATORS_END_ANCHOR_HPP

#include <tuple>

#include <heuristics.hpp>
#include <nodes.hpp>
#include <static_string.hpp>
#include <terminals/anchors/end.hpp>

#include "common.hpp"

namespace e_regex
{
    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'$'>>>
    {
            // End anchor found

            using new_node
                = nodes::basic<terminals::anchors::end, std::tuple<>, 1, 1, nodes::policy::POSSESSIVE>;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_END_ANCHOR_HPP */
