#ifndef E_REGEX_NODES_GROUP_HPP_
#define E_REGEX_NODES_GROUP_HPP_

#include <algorithm>

#include "basic.hpp"
#include "common.hpp"
#include "meta.hpp"
#include "utilities/macros.hpp"
#include "utilities/math.hpp"

namespace e_regex::nodes
{
    template<typename matcher, auto group_index, typename... children>
    struct group
    {
            static constexpr auto expression
                = '(' + matcher::expression + ')'
                  + get_children_expression<children...>();

            using admission_set =
                typename extract_admission_set<matcher>::type;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = e_regex::policy::NONE,
                .minimum_match_size = matcher::meta.minimum_match_size,
                .maximum_match_size = matcher::meta.maximum_match_size};

            static constexpr auto next_group_index = group_index + 1;

            template<typename... injected_children>
            using optimize
                = group<typename matcher::template optimize<children..., injected_children...>,
                        group_index,
                        typename children::template optimize<>...>;

            static constexpr unsigned groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...) + 1;

            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto
                match(auto& result) -> auto&
            {
                auto begin = result.actual_iterator_end;

                // This node's children matter in backtracking
                matcher::template match<children...>(result);

                if (result)
                {
                    result.match_groups[group_index]
                        = literal_string_view {
                            begin, result.actual_iterator_end};

                    return dfs<std::tuple<children...>,
                               std::tuple<children..., injected_children...>>(
                        result);
                }

                return result;
            }
    };
} // namespace e_regex::nodes


#endif /* E_REGEX_NODES_GROUP_HPP_*/
