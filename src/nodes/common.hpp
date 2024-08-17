#ifndef E_REGEX_NODES_COMMON_HPP_
#define E_REGEX_NODES_COMMON_HPP_

#include <algorithm>
#include <tuple>
#include <type_traits>

#include "utilities/admitted_set.hpp"
#include "utilities/first_type.hpp"
#include "utilities/math.hpp"

namespace e_regex::nodes
{
    template<typename... children>
    struct extract_admission_set;

    template<typename child, typename... children>
    struct extract_admission_set<child, children...>
    {
            using type = merge_admitted_sets_t<
                typename decltype(child::meta)::admission_set,
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
    concept has_groups = requires(T t) { t.groups; };

    template<typename T>
    struct group_getter
    {
            static constexpr auto value = 0U;
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

            static constexpr unsigned groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...);
    };

    template<typename injected_children>
    struct invoke_match;

    template<typename... injected_children>
    struct invoke_match<std::tuple<injected_children...>>
    {
            template<typename T>
            static constexpr __attribute__((always_inline)) auto
                match(auto& res) -> auto&
            {
                return T::template match<injected_children...>(res);
            }
    };

    template<typename nodes,
             typename terminals = std::tuple<>,
             typename others    = std::tuple<>>
    struct zipper;

    template<typename node, typename... tail, typename... terminals, typename... others>
    struct zipper<std::tuple<node, tail...>,
                  std::tuple<terminals...>,
                  std::tuple<others...>>
    {};

    template<typename children          = std::tuple<>,
             typename injected_children = std::tuple<>>
    constexpr __attribute__((always_inline)) auto
        dfs(auto& match_result) noexcept -> auto&
    {
        if constexpr (std::tuple_size_v<children> == 0)
        {
            return match_result;
        }
        else
        {
            using _children = first_type<children>;
            using invoker   = invoke_match<injected_children>;

            if constexpr (std::tuple_size_v<children> == 1)
            {
                return invoker::template match<typename _children::type>(
                    match_result);
            }
            else
            {
                if (invoker::template match<typename _children::type>(
                        match_result))
                {
                    return match_result;
                }

                match_result = true;
                return dfs<typename _children::remaining>(match_result);
            }
        }
    }
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_COMMON_HPP_*/
