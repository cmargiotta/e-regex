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
                = static_string {"("} + matcher::expression
                  + static_string {")"}
                  + get_children_expression<children...>();

            using admitted_first_chars =
                typename matcher::admitted_first_chars;
            static constexpr auto next_group_index = group_index + 1;

            static constexpr std::size_t groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...) + 1;

            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                auto begin = result.actual_iterator_end;

                result
                    = matcher::template match<children..., second_layer_children...>(
                        result);

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
