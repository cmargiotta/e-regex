#ifndef E_REGEX_OPERATORS_END_ANCHOR_HPP_
#define E_REGEX_OPERATORS_END_ANCHOR_HPP_

#include <tuple>

#include "common.hpp"
#include "nodes.hpp"
#include "terminals/anchors/end.hpp"
#include "tree_builder/common.hpp"

namespace e_regex
{
    template<typename last_node, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'$'>>, group_index>
    {
            // End anchor found

            using new_node = nodes::simple<terminals::anchors::end>;

            using tree = add_child_t<last_node, new_node>;
    };
} // namespace e_regex

#endif /* E_REGEX_OPERATORS_END_ANCHOR_HPP_*/
