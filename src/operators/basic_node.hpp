#ifndef OPERATORS_BASIC_NODE_HPP
#define OPERATORS_BASIC_NODE_HPP

#include <algorithm>
#include <tuple>
#include <type_traits>

#include "match_result.hpp"
#include "static_string.hpp"
#include "terminals/common.hpp"
#include "utilities/sum.hpp"

namespace e_regex
{
    enum class policy
    {
        GREEDY,
        LAZY,
        POSSESSIVE
    };

    template<typename Child, typename... Children>
    constexpr auto dfs(auto match_result) noexcept
    {
        if constexpr (sizeof...(Children) == 0)
        {
            return Child::match(std::move(match_result));
        }
        else
        {
            auto result = Child::match(match_result);

            match_result.matches += Child::groups;
            match_result = dfs<Children...>(std::move(match_result));

            if (!result || (result.actual_iterator_end < match_result.actual_iterator_end))
            {
                return match_result;
            }

            return result;
        }
    }

    template<typename T>
    concept has_groups = requires(T t) { t.groups; };

    template<typename T>
    struct group_getter
    {
            static constexpr auto value = 0;
    };

    template<has_groups T>
    struct group_getter<T>
    {
            static constexpr auto value = T::groups;
    };

    template<typename matcher,
             typename children             = std::tuple<>,
             std::size_t repetitions_min   = 1,
             std::size_t repetitions_max   = 1,
             policy      repetition_policy = policy::GREEDY,
             bool        grouping          = false>
    struct basic_node;

    template<typename matcher, bool grouping_>
    struct set_node_grouping;

    template<typename matcher, typename children, std::size_t min, std::size_t max, policy policy_, bool grouping, bool grouping_>
    struct set_node_grouping<basic_node<matcher, children, min, max, policy_, grouping>, grouping_>
    {
            using type = basic_node<matcher, children, min, max, policy_, grouping_>;
    };

    template<typename matcher, std::size_t min, std::size_t max, policy policy_>
    struct set_node_range;

    template<typename matcher,
             typename children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping,
             std::size_t min,
             std::size_t max,
             policy      policy_>
    struct set_node_range<basic_node<matcher, children, repetitions_min, repetitions_max, repetition_policy, grouping>,
                          min,
                          max,
                          policy_>
    {
            using type = basic_node<matcher, children, min, max, policy_, grouping>;
    };

    template<typename matcher, bool grouping_>
    using set_node_grouping_t = typename set_node_grouping<matcher, grouping_>::type;

    template<typename matcher, std::size_t min, std::size_t max, policy policy_>
    using set_node_range_t = typename set_node_range<matcher, min, max, policy_>::type;

    template<typename matcher, typename children = std::tuple<>, policy policy_ = matcher::backtracking_policy>
    using grouping_node = basic_node<matcher, children, 1, 1, policy_, true>;

    template<typename matcher, typename children = std::tuple<>, policy policy_ = policy::GREEDY>
    using optional_node = basic_node<matcher, children, 0, 1, policy_, false>;

    template<typename node>
    struct get_expression;

    template<typename... terminals>
    struct get_expression<e_regex::terminals::terminal<terminals...>>
    {
            using type =
                typename e_regex::terminals::rebuild_expression<e_regex::terminals::terminal<terminals...>>::string;
    };

    template<typename matcher, std::size_t repetitions_min, std::size_t repetitions_max, policy repetition_policy, bool grouping>
    struct get_expression<
        basic_node<matcher, std::tuple<>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using type = typename get_expression<matcher>::type;
    };

    template<typename... children, std::size_t repetitions_min, std::size_t repetitions_max, policy repetition_policy, bool grouping>
    struct get_expression<
        basic_node<void, std::tuple<children...>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using type
                = concatenate_pack_strings_t<pack_string<'|'>, typename get_expression<children>::type...>;
    };

    template<typename... matchers,
             typename child,
             typename... children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping>
    struct get_expression<
        basic_node<terminals::terminal<matchers...>, std::tuple<child, children...>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using matcher_string = merge_pack_strings_t<
                typename terminals::rebuild_expression<terminals::terminal<matchers...>>::string,
                typename get_expression<child>::type>;

            using type = concatenate_pack_strings_t<pack_string<'|'>,
                                                    matcher_string,

                                                    typename get_expression<children>::type...>;
    };

    template<typename node>
    using get_expression_t = typename get_expression<node>::type;
}// namespace e_regex

#endif /* OPERATORS_BASIC_NODE_HPP */
