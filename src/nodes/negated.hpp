#ifndef E_REGEX_NODES_NEGATED_HPP_
#define E_REGEX_NODES_NEGATED_HPP_

#include <utility>

#include "basic.hpp"
#include "nodes/get_expression.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct negated_node : public base<matcher>
    {
            static constexpr auto expression
                = "[^" + matcher::expression + "]";
            using admitted_first_chars
                = admitted_set_complement_t<typename matcher::admitted_first_chars>;

            template<typename... injected_children>
            using optimize
                = negated_node<typename matcher::template optimize<>>;

            template<typename... injected_children>
            static constexpr auto match(auto& result) -> auto&
            {
                if (result.actual_iterator_end >= result.query.end())
                {
                    result = false;
                    return result;
                }

                matcher::match(result);
                result.accepted = !result.accepted;

                return result;
            }
    };

    // TODO missing get_expression
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_NEGATED_HPP_*/
