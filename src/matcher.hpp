#ifndef MATCHER
#define MATCHER

#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "heuristics.hpp"
#include "match_result.hpp"
#include "utilities/max.hpp"

namespace e_regex
{
    template<typename T>
    concept has_groups = requires(T t)
    {
        t.groups;
    };

    template<typename matcher, typename children = std::tuple<>>
    struct basic_node;

    template<typename matcher, typename... children>
    struct basic_node<matcher, std::tuple<children...>>
    {
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

            static constexpr std::size_t groups
                = group_getter<matcher>::value + max(children::groups...);

            template<typename Child, typename... Children>
            static constexpr auto dfs(auto match_result) noexcept
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

            static constexpr auto match(auto result)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    result = matcher::match(std::move(result));
                }

                if constexpr (sizeof...(children) != 0)
                {
                    if (result)
                    {
                        return dfs<children...>(std::move(result));
                    }
                }

                return result;
            }
    };

}// namespace e_regex

#endif /* MATCHER */
