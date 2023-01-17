#ifndef OPERATORS_BRACES
#define OPERATORS_BRACES

#include <limits>

#include "common.hpp"
#include "greedy_node.hpp"
#include "operators/basic_node.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/pack_string_to_number.hpp"

namespace e_regex
{
    template<typename matcher, typename data>
    struct quantified_node_builder;

    template<typename matcher, typename first>
    struct quantified_node_builder<matcher, std::tuple<first>>
    {
            static constexpr auto value = pack_string_to_number<first>::value;
            using type                  = set_node_range_t<matcher, value, value, policy::GREEDY>;
    };

    template<typename matcher, typename first>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>>
    {
            using type = set_node_range_t<matcher,
                                          pack_string_to_number<first>::value,
                                          std::numeric_limits<std::size_t>::max(),
                                          policy::GREEDY>;
    };

    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>>
    {
            using type = set_node_range_t<matcher,
                                          pack_string_to_number<first>::value,
                                          pack_string_to_number<second>::value,
                                          policy::GREEDY>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using new_node =
                typename quantified_node_builder<last_node, typename substring::result>::type;

            using tree = typename tree_builder_helper<new_node, typename substring::remaining>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES */
