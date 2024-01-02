#ifndef SRC_NODES_NEGATED_NODE_HPP
#define SRC_NODES_NEGATED_NODE_HPP

#include <utility>

#include "basic.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct negated_node : public base<matcher>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                result = matcher::template match<second_layer_children...>(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };
}// namespace e_regex::nodes

#endif /* SRC_NODES_NEGATED_NODE_HPP */
