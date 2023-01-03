#ifndef MATCHER
#define MATCHER

#include <string_view>
#include <tuple>
#include <type_traits>

#include "match_result.hpp"

namespace e_regex
{
    template<typename... node>
    struct group_counter
    {
            static constexpr std::size_t value = 0;
    };

    template<typename node, typename... nodes>
    struct group_counter<node, nodes...>
    {
            static constexpr std::size_t value
                = group_counter<node>::value + group_counter<nodes...>::value;
    };

    template<typename node>
    struct group_counter<node>
    {
            static constexpr std::size_t value = 0;
    };

    template<typename matcher, bool repeat, bool optional, bool group, typename child>
    class tree_node : public tree_node<matcher, repeat, optional, group, std::tuple<child>>
    {
    };

    template<typename matcher, bool repeat, bool optional, typename... children>
    struct group_counter<tree_node<matcher, repeat, optional, false, std::tuple<children...>>>
    {
            static constexpr std::size_t value = group_counter<children...>::value;
    };

    template<bool repeat, bool optional, typename... children, typename matcher, bool repeat_, bool optional_, bool group_, typename tuple>
    struct group_counter<
        tree_node<tree_node<matcher, repeat_, optional_, group_, tuple>, repeat, optional, true, std::tuple<children...>>>
    {
            static constexpr std::size_t value
                = 1 + group_counter<tree_node<matcher, repeat_, optional_, group_, tuple>>::value
                  + group_counter<children...>::value;
    };

    template<typename matcher, bool repeat, bool optional, bool group, typename... children>
    struct tree_node<matcher, repeat, optional, group, std::tuple<children...>>
    {
            static constexpr std::size_t groups = group_counter<tree_node>::value;

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

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if (query_iterator == end)
                {
                    result = optional;
                    return result;
                }

                if constexpr (!std::is_same_v<matcher, void>)
                {
                    unsigned    matches       = 0;
                    std::size_t match_index   = 0;
                    auto        last_iterator = query_iterator;

                    if constexpr (group)
                    {
                        result.matches++;
                        match_index = result.matches;
                    }

                    if constexpr (repeat)
                    {
                        while (query_iterator < end)
                        {
                            auto res = matcher::match(query_iterator, end, result);

                            if (res)
                            {
                                result = res;
                                matches++;
                                last_iterator  = query_iterator;
                                query_iterator = result.actual_iterator_end;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if constexpr (!optional)
                        {
                            result = matches > 0;
                        }
                    }
                    else
                    {
                        result = matcher::match(query_iterator, end, std::move(result));

                        last_iterator  = query_iterator;
                        query_iterator = result.actual_iterator_end;
                    }

                    if constexpr (group)
                    {
                        if (result)
                        {
                            result.match_groups[match_index - 1]
                                = std::string_view {last_iterator, query_iterator};
                        }
                    }
                }

                if (result)
                {
                    return dfs<Iterator, children...>(query_iterator, end, std::move(result));
                }

                return result;
            }

            static constexpr auto match(std::string_view query)
            {
                return match_result<tree_node, groups> {query};
            }
    };

    template<typename node, typename child>
    struct add_child;

    template<typename matcher, bool repeat, bool optional, bool group, typename... children, typename child>
    struct add_child<tree_node<matcher, repeat, optional, group, std::tuple<children...>>, child>
    {
            using type = tree_node<matcher, repeat, optional, group, std::tuple<children..., child>>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;

    template<typename node, bool value>
    struct set_repetition;

    template<typename matcher, bool repeat, bool optional, bool group, typename... children, bool value>
    struct set_repetition<tree_node<matcher, repeat, optional, group, std::tuple<children...>>, value>
    {
            using type = tree_node<matcher, value, optional, group, std::tuple<children...>>;
    };

    template<typename node, bool value>
    using set_repetition_t = typename set_repetition<node, value>::type;

    template<typename node, bool value>
    struct set_optional;

    template<typename matcher, bool repeat, bool optional, bool group, typename... children, bool value>
    struct set_optional<tree_node<matcher, repeat, optional, group, std::tuple<children...>>, value>
    {
            using type = tree_node<matcher, repeat, value, group, std::tuple<children...>>;
    };

    template<typename node, bool value>
    using set_optional_t = typename set_optional<node, value>::type;

    template<typename node, bool value>
    struct set_group;

    template<typename matcher, bool repeat, bool optional, bool group, typename... children, bool value>
    struct set_group<tree_node<matcher, repeat, optional, group, std::tuple<children...>>, value>
    {
            using type = tree_node<matcher, repeat, optional, value, std::tuple<children...>>;
    };

    template<typename node, bool value>
    using set_group_t = typename set_group<node, value>::type;
}// namespace e_regex

#endif /* MATCHER */
