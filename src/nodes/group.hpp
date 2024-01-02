#ifndef NODES_GROUP_HPP
#define NODES_GROUP_HPP

#include <algorithm>

#include "basic.hpp"
#include "static_string.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename matcher, auto group_index, typename... children>
    struct group
    {
            static constexpr auto next_group_index = group_index + 1;

            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(group_getter<children>::value...) + 1;

            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                auto begin = result.actual_iterator_end;

                result = matcher::template match<children..., second_layer_children...>(result);

                if (result)
                {
                    result.match_groups[group_index]
                        = literal_string_view {begin, result.actual_iterator_end};

                    result = dfs<children...>(result);
                }

                return result;
            }
    };

    template<typename matcher, auto index, typename... children>
    struct get_expression<group<matcher, index, children...>>
    {
            using type
                = concatenate_pack_strings_t<pack_string<>,
                                             pack_string<'('>,
                                             typename get_expression_base<matcher, children...>::type,
                                             pack_string<')'>>;
    };
}// namespace e_regex::nodes

#endif /* NODES_GROUP_HPP */
