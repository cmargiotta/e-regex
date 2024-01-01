#ifndef OPERATORS_BRACES
#define OPERATORS_BRACES

#include <limits>

#include <nodes.hpp>

#include "common.hpp"
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

    template<typename matcher, typename data, policy policy_ = policy::GREEDY>
    struct quantified_node_builder;

    template<typename matcher, typename first, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first>, policy_>
    {
            static constexpr auto value = pack_string_to_number<first>::value;
            using type                  = set_node_range_t<matcher, value, value, policy_>;
    };

    template<typename matcher, typename first, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, policy_>
    {
            using type = set_node_range_t<matcher,
                                          pack_string_to_number<first>::value,
                                          std::numeric_limits<std::size_t>::max(),
                                          policy_>;
    };

    template<typename matcher, typename first, typename second, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, policy_>
    {
            using type
                = set_node_range_t<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value, policy_>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using remaining_head = first_type<typename substring::remaining>;

            static constexpr auto policy_
                = std::is_same_v<typename remaining_head::type, pack_string<'?'>>
                      ? policy::LAZY
                      : (std::is_same_v<typename remaining_head::type, pack_string<'+'>>
                             ? policy::POSSESSIVE
                             : policy::GREEDY);

            using remaining = std::conditional_t<policy_ != policy::GREEDY,
                                                 typename remaining_head::remaining,
                                                 typename substring::remaining>;

            using new_node =
                typename quantified_node_builder<last_node, typename substring::result, policy_>::type;

            using tree = typename tree_builder_helper<new_node, remaining>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES */
