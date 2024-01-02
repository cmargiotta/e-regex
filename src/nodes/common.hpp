#ifndef NODES_COMMON_HPP
#define NODES_COMMON_HPP

#include <algorithm>

#include "utilities/max.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename T>
    concept node_with_second_layer_children = requires() {
        // template match (auto) -> auto

        T::template match<void, void>;
    };

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

    template<typename T>
    concept has_group_index = requires(T t) { t.next_group_index; };

    template<typename T>
    struct group_index_getter
    {
            static constexpr auto value = 0;
    };

    template<has_groups T>
    struct group_index_getter<T>
    {
            static constexpr auto value = T::next_group_index;
    };

    template<typename matcher, typename... children>
    struct base
    {
            static constexpr auto next_group_index
                = max(group_index_getter<matcher>::value, group_index_getter<children>::value...);

            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(group_getter<children>::value...);
    };

    constexpr auto dfs(auto match_result) noexcept
    {
        return match_result;
    }

    template<typename Child, typename... Children>
    constexpr auto dfs(auto match_result) noexcept
    {
        if (!match_result)
        {
            return match_result;
        }

        if constexpr (sizeof...(Children) == 0)
        {
            return Child::match(std::move(match_result));
        }
        else
        {
            auto result = Child::match(match_result);

            match_result = dfs<Children...>(std::move(match_result));

            if (!result || (result.actual_iterator_end < match_result.actual_iterator_end))
            {
                return match_result;
            }

            return result;
        }
    }
}// namespace e_regex::nodes

#endif /* NODES_COMMON_HPP */
