#ifndef E_REGEX_NODES_NEGATED_HPP_
#define E_REGEX_NODES_NEGATED_HPP_

#include <utility>

#include "basic.hpp"
#include "common.hpp"
#include "nodes/get_expression.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct negated_node : public base<matcher>
    {
            static constexpr auto expression
                = "[^" + matcher::expression + "]";

            using admission_set = admitted_set_complement_t<
                typename extract_admission_set<matcher>::type>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = policy::NONE,
                .minimum_match_size = matcher::meta.minimum_match_size,
                .maximum_match_size = matcher::meta.minimum_match_size,
            };

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
