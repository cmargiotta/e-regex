#ifndef OPERATORS_BASIC_NODE
#define OPERATORS_BASIC_NODE

#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "match_result.hpp"
#include "utilities/max.hpp"

namespace e_regex
{
    enum class policy
    {
        GREEDY,
        LAZY
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

            if (!result)
            {
                return dfs<Children...>(std::move(match_result));
            }

            return result;
        }
    }

    template<typename T>
    concept has_groups = requires(T t)
    {
        t.groups;
    };

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

    template<typename matcher, typename children = std::tuple<>, policy policy_ = policy::GREEDY>
    using grouping_node = basic_node<matcher, children, 1, 1, policy_, true>;

    template<typename matcher, typename children = std::tuple<>, policy policy_ = policy::GREEDY>
    using optional_node = basic_node<matcher, children, 0, 1, policy_, false>;
}// namespace e_regex

#endif /* OPERATORS_BASIC_NODE */
