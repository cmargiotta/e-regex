#ifndef OPERATORS_BRACES_HPP
#define OPERATORS_BRACES_HPP

#include <limits>

#include "common.hpp"
#include "nodes.hpp"
#include "nodes/basic.hpp"
#include "nodes/greedy.hpp"
#include "nodes/repeated.hpp"
#include "static_string.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/pack_string_to_number.hpp"

namespace e_regex
{
    template<typename data>
    struct first_type;

    template<typename head, typename... tail>
    struct first_type<std::tuple<head, tail...>>
    {
            using type      = head;
            using remaining = std::tuple<tail...>;
    };

    template<>
    struct first_type<std::tuple<>>
    {
            using type      = void;
            using remaining = std::tuple<>;
    };

    template<typename data>
    using first_type_t = typename first_type<data>::type;

    template<typename matcher, typename data, typename policy_>
    struct quantified_node_builder;

    template<typename matcher, typename first, typename policy>
    struct quantified_node_builder<matcher, std::tuple<first>, policy>
    {
            // Exact quantifier e.g. a{3}
            static constexpr auto value = pack_string_to_number<first>::value;
            using type                  = nodes::repeated<matcher, value>;
    };

    // {n,} quantifiers
    template<typename matcher, typename first>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, pack_string<'?'>>
    {
            using type = nodes::lazy<matcher, pack_string_to_number<first>::value>;
    };

    template<typename matcher, typename first>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, pack_string<'+'>>
    {
            using type = nodes::possessive<matcher, pack_string_to_number<first>::value>;
    };

    template<typename matcher, typename first, typename policy>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, policy>
    {
            using type = nodes::greedy<matcher, pack_string_to_number<first>::value>;
    };

    // {n, n} quantifiers
    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, pack_string<'?'>>
    {
            using type
                = nodes::lazy<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value>;
    };

    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, pack_string<'+'>>
    {
            using type
                = nodes::possessive<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value>;
    };

    template<typename matcher, typename first, typename second, typename policy>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, policy>
    {
            using type
                = nodes::greedy<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>, group_index>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using remaining_head = first_type<typename substring::remaining>;
            using policy         = typename remaining_head::type;

            using remaining
                = std::conditional_t<std::is_same_v<typename remaining_head::type, pack_string<'+'>>
                                         || std::is_same_v<typename remaining_head::type, pack_string<'?'>>,
                                     typename remaining_head::remaining,
                                     typename substring::remaining>;

            using new_node =
                typename quantified_node_builder<last_node, typename substring::result, policy>::type;

            using tree = typename tree_builder_helper<new_node, remaining, group_index>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES_HPP */
