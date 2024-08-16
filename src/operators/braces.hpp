#ifndef E_REGEX_OPERATORS_BRACES_HPP_
#define E_REGEX_OPERATORS_BRACES_HPP_

#include <limits>

#include "common.hpp"
#include "nodes.hpp"
#include "nodes/basic.hpp"
#include "nodes/greedy.hpp"
#include "nodes/repeated.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/first_type.hpp"
#include "utilities/pack_string_to_number.hpp"

namespace e_regex
{
    template<typename matcher, typename data, typename policy_>
    struct quantified_node_builder;

    template<typename matcher, typename first, typename policy>
    struct quantified_node_builder<matcher, std::tuple<first>, policy>
    {
            // Exact quantifier e.g. a{3}

            using type
                = nodes::repeated<matcher, pack_string_to_number<first>::value>;
    };

    // {n,} quantifiers
    template<typename matcher, typename first>
    struct quantified_node_builder<matcher,
                                   std::tuple<first, pack_string<','>>,
                                   pack_string<'?'>>
    {
            using type
                = nodes::lazy<matcher, pack_string_to_number<first>::value>;
    };

    template<typename matcher, typename first>
    struct quantified_node_builder<matcher,
                                   std::tuple<first, pack_string<','>>,
                                   pack_string<'+'>>
    {
            using type
                = nodes::possessive<matcher, pack_string_to_number<first>::value>;
    };

    template<typename matcher, typename first, typename policy>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, policy>
    {
            using type
                = nodes::greedy<matcher, pack_string_to_number<first>::value>;
    };

    // {n, n} quantifiers
    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher,
                                   std::tuple<first, pack_string<','>, second>,
                                   pack_string<'?'>>
    {
            using type
                = nodes::lazy<matcher,
                              pack_string_to_number<first>::value,
                              pack_string_to_number<second>::value>;
    };

    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher,
                                   std::tuple<first, pack_string<','>, second>,
                                   pack_string<'+'>>
    {
            using type
                = nodes::possessive<matcher,
                                    pack_string_to_number<first>::value,
                                    pack_string_to_number<second>::value>;
    };

    template<typename matcher, typename first, typename second, typename policy>
    struct quantified_node_builder<matcher,
                                   std::tuple<first, pack_string<','>, second>,
                                   policy>
    {
            using type
                = nodes::greedy<matcher,
                                pack_string_to_number<first>::value,
                                pack_string_to_number<second>::value>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>, group_index>
    {
            // { found
            using substring
                = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            // Get first character after }, can be + or ? for
            // specifying the policy
            using remaining_head
                = first_type<typename substring::remaining>;
            using policy = typename remaining_head::type;

            // If a policy is specified, remaining is everything after
            // remaining_head, otherwise it is everything after }
            using remaining = std::conditional_t<
                std::is_same_v<typename remaining_head::type, pack_string<'+'>>
                    || std::is_same_v<typename remaining_head::type, pack_string<'?'>>,
                typename remaining_head::remaining,
                typename substring::remaining>;

            using new_node =
                typename quantified_node_builder<last_node,
                                                 typename substring::result,
                                                 policy>::type;

            using tree =
                typename tree_builder_helper<new_node, remaining, group_index>::tree;
    };
} // namespace e_regex

#endif /* E_REGEX_OPERATORS_BRACES_HPP_*/
