#ifndef E_REGEX_OPERATORS_ROUND_BRACKETS_HPP_
#define E_REGEX_OPERATORS_ROUND_BRACKETS_HPP_

#include "common.hpp"
#include "nodes.hpp"
#include "nodes/basic.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/split.hpp"

namespace e_regex
{
    template<typename parsed, typename branches, auto group_index>
    struct branched;

    template<typename... parsed, typename subregex, typename... subregexes, auto group_index>
    struct branched<std::tuple<parsed...>, std::tuple<subregex, subregexes...>, group_index>
    {
            using subtree =
                typename tree_builder_helper<void, subregex, group_index>::tree;

            using tree =
                typename branched<std::tuple<parsed..., subtree>,
                                  std::tuple<subregexes...>,
                                  group_index>::tree;
    };

    template<typename... parsed, auto group_index>
    struct branched<std::tuple<parsed...>, std::tuple<>, group_index>
    {
            using tree = nodes::simple<void, parsed...>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'('>, tail...>, group_index>
    {
            // ( found
            using substring
                = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex =
                typename branched<std::tuple<>,
                                  split_t<'|', typename substring::result>,
                                  group_index + 1>::tree;

            using node = nodes::group<subregex, group_index>;

            using new_node =
                typename tree_builder_helper<node,
                                             typename substring::remaining,
                                             node::next_group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'('>, pack_string<'?'>, pack_string<':'>, tail...>,
        group_index>
    {
            // Non capturing group found
            using substring
                = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = nodes::simple<
                typename branched<std::tuple<>,
                                  split_t<'|', typename substring::result>,
                                  group_index>::tree>;

            using new_node =
                typename tree_builder_helper<subregex,
                                             typename substring::remaining,
                                             subregex::next_group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
} // namespace e_regex

#endif /* E_REGEX_OPERATORS_ROUND_BRACKETS_HPP_*/
