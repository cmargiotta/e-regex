#ifndef NODES_BASIC_HPP
#define NODES_BASIC_HPP

#include <type_traits>

#include "common.hpp"
#include "get_expression.hpp"
#include "static_string.hpp"
#include "terminals.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct admitted_first_chars_getter
    {
            using type = typename matcher::admitted_first_chars;
    };

    template<>
    struct admitted_first_chars_getter<void>
    {
            using type = admitted_set<char>;
    };

    template<typename matcher, typename... children>
    struct simple : public base<matcher, children...>
    {
            using expression           = typename get_expression_base<matcher, children...>::type;
            using admitted_first_chars = typename extract_admission_set<matcher, children...>::type;

            template<typename... second_layer_children>
            static constexpr auto match(auto res)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    res = matcher::template match<second_layer_children...>(res);
                }

                return dfs<children...>(res);
            }
    };

    template<typename child>
    struct simple<void, child> : public child
    {
            using expression = typename get_expression_base<child>::type;
    };

    template<node_with_second_layer_children matcher, typename... children>
    struct simple<matcher, children...> : public base<matcher, children...>
    {
            using expression           = typename get_expression_base<matcher, children...>::type;
            using admitted_first_chars = typename matcher::admitted_first_chars;

            template<typename... second_layer_children>
            static constexpr auto match(auto res)
            {
                res = matcher::template match<second_layer_children...>(res);
                return dfs<children...>(res);
            }
    };
}// namespace e_regex::nodes

#endif /* NODES_BASIC_HPP */
