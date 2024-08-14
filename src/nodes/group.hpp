#ifndef E_REGEX_NODES_GROUP_HPP_
#define E_REGEX_NODES_GROUP_HPP_

#include <algorithm>

#include "basic.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename matcher, auto group_index, typename... children>
    struct group
    {
            static constexpr auto expression
                = '(' + matcher::expression + ')'
                  + get_children_expression<children...>();

            using admitted_first_chars =
                typename matcher::admitted_first_chars;
            static constexpr auto next_group_index = group_index + 1;

            template<typename... injected_children>
            using optimize
                = group<typename matcher::template optimize<children..., injected_children...>,
                        group_index,
                        typename children::template optimize<>...>;

            static constexpr std::size_t groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...) + 1;

            template<typename... injected_children>
            static constexpr auto match(auto result)
            {
                auto begin = result.actual_iterator_end;

                // This node's children matter in backtracking
                result = matcher::template match<children...>(result);

                if (result)
                {
                    result.match_groups[group_index]
                        = literal_string_view {
                            begin, result.actual_iterator_end};

                    result = dfs<children...>(result);
                }

                return result;
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_GROUP_HPP_*/
