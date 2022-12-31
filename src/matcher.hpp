#ifndef MATCHER
#define MATCHER

#include <array>
#include <string_view>
#include <tuple>
#include <type_traits>

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

    template<std::size_t groups>
    struct match_result
    {
            std::array<std::string_view, groups + 1> match_groups;
            std::size_t                              matches  = 0;
            bool                                     accepted = true;

            constexpr auto operator=(bool accepted) noexcept -> match_result&
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return accepted;
            }

            constexpr auto is_accepted() const noexcept
            {
                return accepted;
            }

            constexpr auto operator[](std::size_t index) const noexcept
            {
                return match_groups[index];
            }

            constexpr auto to_view() const noexcept
            {
                return match_groups[0];
            }

            constexpr operator std::string_view() const noexcept
            {
                return to_view();
            }

            constexpr auto size() const noexcept
            {
                return matches;
            }
    };

    template<typename T>
    concept is_node = requires(T t)
    {
        t.groups;
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
            static constexpr auto dfs(Iterator /*unused*/, Iterator end, auto result)
            {
                return std::make_pair(std::move(result), end);
            }

            template<typename Iterator, typename Child, typename... Children>
            static constexpr auto dfs(Iterator query_iterator, Iterator end, auto match_result)
            {
                auto [result, iterator] = Child::match(query_iterator, end, match_result);

                if (!result && sizeof...(Children) > 0)
                {
                    return dfs<Iterator, Children...>(query_iterator, end, std::move(match_result));
                }

                return std::make_pair(std::move(result), iterator);
            }

            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    unsigned    matches       = 0;
                    std::size_t match_index   = 0;
                    auto        last_iterator = query_iterator;

                    if constexpr (is_node<matcher>)
                    {
                        result.matches++;
                        match_index = result.matches;
                    }

                    if constexpr (repeat)
                    {
                        while (query_iterator < end)
                        {
                            auto res = matcher::match(query_iterator, end, std::move(result));

                            result = std::move(res.first);

                            if (result)
                            {
                                matches++;
                                last_iterator  = query_iterator;
                                query_iterator = res.second;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if constexpr (optional)
                        {
                            result = true;
                        }
                        else
                        {
                            result = matches > 0;
                        }
                    }
                    else
                    {
                        auto res = matcher::match(query_iterator, end, result);

                        result         = std::move(res.first);
                        last_iterator  = query_iterator;
                        query_iterator = res.second;
                    }

                    if (result)
                    {
                        if constexpr (is_node<matcher>)
                        {
                            result.match_groups[match_index]
                                = std::string_view {last_iterator, query_iterator};
                        }
                    }
                }

                if (result)
                {
                    if constexpr (sizeof...(children) > 0)
                    {
                        return dfs<Iterator, children...>(query_iterator, end, std::move(result));
                    }
                }

                return std::make_pair(std::move(result), query_iterator);
            }

            static constexpr auto match(std::string_view query)
            {
                auto [result, iterator] = match(query.begin(), query.end(), match_result<groups> {});

                result.match_groups[0] = std::string_view {query.begin(), iterator};

                return result;
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
