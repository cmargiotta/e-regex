#ifndef E_REGEX_NODES_COMMON_HPP_
#define E_REGEX_NODES_COMMON_HPP_

#include <algorithm>

#include "utilities/admitted_set.hpp"
#include "utilities/max.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename... children>
    struct extract_admission_set;

    template<typename child, typename... children>
    struct extract_admission_set<child, children...>
    {
            using type = merge_admitted_sets_t<
                typename child::admitted_first_chars,
                typename extract_admission_set<children...>::type>;
    };

    template<typename... children>
    struct extract_admission_set<void, children...>
    {
            using type =
                typename extract_admission_set<children...>::type;
    };

    template<>
    struct extract_admission_set<>
    {
            using type = admitted_set<char>;
    };

    template<typename T>
    concept node_with_injected_children = requires() {
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
                = max(group_index_getter<matcher>::value,
                      group_index_getter<children>::value...);

            static constexpr std::size_t groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...);
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
            auto result
                = Child::template match<Children...>(match_result);

            match_result = dfs<Children...>(std::move(match_result));

            if (!result
                || (match_result
                    && result.actual_iterator_end
                           < match_result.actual_iterator_end))
            {
                return match_result;
            }

            return result;
        }
    }
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_COMMON_HPP_*/
