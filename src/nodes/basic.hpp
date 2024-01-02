#ifndef NODES_BASIC_HPP
#define NODES_BASIC_HPP

#include "common.hpp"
#include "static_string.hpp"
#include "terminals.hpp"

namespace e_regex::nodes
{
    template<typename matcher, typename... children>
    struct simple : public base<matcher, children...>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto res)
            {
                if constexpr (!std::is_same_v<matcher, void>)
                {
                    res = matcher::template match(res);
                }

                return dfs<children...>(res);
            }
    };

    template<node_with_second_layer_children matcher, typename... children>
    struct simple<matcher, children...> : public base<matcher, children...>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto res)
            {
                res = matcher::template match<second_layer_children...>(res);
                return dfs<children...>(res);
            }
    };
}// namespace e_regex::nodes

#endif /* NODES_BASIC_HPP */
