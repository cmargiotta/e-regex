#ifndef SRC_NODES_NEGATED_NODE_HPP
#define SRC_NODES_NEGATED_NODE_HPP

#include <utility>

#include "basic.hpp"
#include "nodes/get_expression.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct negated_node : public base<matcher>
    {
            using expression = typename get_expression_base<matcher>::type;
            using admitted_first_chars
                = admitted_set_complement_t<typename matcher::admitted_first_chars>;

            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                result = matcher::template match<second_layer_children...>(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };

    // TODO missing get_expression
}// namespace e_regex::nodes

#endif /* SRC_NODES_NEGATED_NODE_HPP */
