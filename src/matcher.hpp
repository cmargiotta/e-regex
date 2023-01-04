#ifndef MATCHER
#define MATCHER

#include <algorithm>
#include <string_view>
#include <tuple>
#include <type_traits>

#include "match_result.hpp"
#include "utility.hpp"

namespace e_regex
{
    struct base_tree_matcher
    {
            template<typename Iterator>
            static constexpr auto dfs(Iterator /*unused*/, Iterator, auto result)
            {
                return result;
            }

            template<typename Iterator, typename Child, typename... Children>
            static constexpr auto dfs(Iterator query_iterator, Iterator end, auto match_result)
            {
                auto result = Child::match(query_iterator, end, match_result);

                if (!result && sizeof...(Children) > 0)
                {
                    return dfs<Iterator, Children...>(query_iterator, end, std::move(match_result));
                }

                return result;
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

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    result = matcher::match(query_iterator, end, std::move(result));
                }

                if (result)
                {
                    return base_tree_matcher::dfs<Iterator, children...>(
                        result.actual_iterator_end, end, std::move(result));
                }

                return result;
            }
    };

    template<typename matcher>
    struct repeated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if (query_iterator != end)
                {
                    unsigned matches = 0;

                    while (query_iterator < end)
                    {
                        auto res = matcher::match(query_iterator, end, result);

                        if (res)
                        {
                            result = res;
                            matches++;
                            query_iterator = result.actual_iterator_end;
                        }
                        else
                        {
                            break;
                        }
                    }

                    result = matches > 0;
                }
                else
                {
                    result = false;
                }

                return result;
            }
    };

    template<typename matcher>
    struct optional_node
    {
            static constexpr std::size_t groups = matcher::groups;

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if (query_iterator != end)
                {
                    result = matcher::match(query_iterator, end, std::move(result));
                    result = true;
                }

                return result;
            }
    };

    template<typename matcher>
    struct grouping_node
    {
            static constexpr std::size_t groups = 1 + matcher::groups;

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if (query_iterator != end)
                {
                    auto match_index = result.matches;
                    result.matches++;
                    result = matcher::match(query_iterator, end, std::move(result));

                    if (result)
                    {
                        result.match_groups[match_index]
                            = std::string_view {query_iterator, result.actual_iterator_end};
                    }
                }
                else
                {
                    result = false;
                }

                return result;
            }
    };

    template<typename node, typename child>
    struct add_child;

    template<typename matcher, typename... children, typename child>
    struct add_child<basic_node<matcher, std::tuple<children...>>, child>
    {
            using type = basic_node<matcher, std::tuple<children..., child>>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;

}// namespace e_regex

#endif /* MATCHER */
