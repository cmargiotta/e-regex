#ifndef MATCHER
#define MATCHER

#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <heuristics.hpp>
#include <match_result.hpp>
#include <utilities/max.hpp>

namespace e_regex
{
    struct base_tree_matcher
    {
            static constexpr auto dfs(auto result) noexcept
            {
                return result;
            }

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

                    if (!result && sizeof...(Children) > 0)
                    {
                        return dfs<Children...>(std::move(match_result));
                    }

                    return result;
                }
            }
    };

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

            static constexpr auto match(auto result)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    result = matcher::match(std::move(result));
                }

                if (result)
                {
                    return base_tree_matcher::dfs<children...>(std::move(result));
                }

                return result;
            }
    };

    template<typename matcher>
    struct repeated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                unsigned matches = 0;

                while (result.actual_iterator_end < result.query.end())
                {
                    auto res = matcher::match(result);

                    if (res)
                    {
                        result = std::move(res);
                        matches++;
                    }
                    else
                    {
                        break;
                    }
                }

                result = matches > 0;

                return result;
            }
    };

    template<typename matcher>
    struct optional_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                auto res = matcher::match(result);

                if (res)
                {
                    result = res;
                }

                return result;
            }
    };

    template<typename matcher>
    struct negated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                result = matcher::match(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };

    template<typename matcher>
    struct grouping_node
    {
            static constexpr std::size_t groups = 1 + matcher::groups;

            static constexpr auto match(auto result)
            {
                auto begin       = result.actual_iterator_end;
                auto match_index = result.matches;
                result.matches++;
                result = matcher::match(std::move(result));

                if (result)
                {
                    result.match_groups[match_index]
                        = std::string_view {begin, result.actual_iterator_end};
                }

                return result;
            }
    };
}// namespace e_regex

#endif /* MATCHER */
