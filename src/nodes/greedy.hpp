#ifndef E_REGEX_NODES_GREEDY_HPP_
#define E_REGEX_NODES_GREEDY_HPP_

#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include "common.hpp"
#include "get_expression.hpp"
#include "nodes/possessive.hpp"
#include "possessive.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/literal_string_view.hpp"
#include "utilities/number_to_pack_string.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max
             = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct greedy : public base<matcher, children...>
    {
            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(children::groups...);

            static constexpr auto expression = []() {
                auto quantifier = []() {
                    if constexpr (repetitions_min == 0
                                  && repetitions_max
                                         == std::numeric_limits<std::size_t>::max())
                    {
                        return static_string {"*"};
                    }
                    else if constexpr (repetitions_min == 1
                                       && repetitions_max
                                              == std::numeric_limits<
                                                  std::size_t>::max())
                    {
                        return static_string {"+"};
                    }
                    else if constexpr (repetitions_min == 0
                                       && repetitions_max == 1)
                    {
                        return static_string {"?"};
                    }
                    else if constexpr (repetitions_max
                                       == std::numeric_limits<std::size_t>::max())
                    {
                        return static_string {"{"}
                               + number_to_pack_string_t<repetitions_min>::string
                               + static_string {",}"};
                    }
                    else
                    {
                        return static_string {"{"}
                               + number_to_pack_string_t<repetitions_min>::string
                               + ','
                               + number_to_pack_string_t<repetitions_max>::string
                               + '}';
                    }
                }();

                auto self = matcher::expression;

                if constexpr (sizeof...(children) <= 1)
                {
                    return self + quantifier
                           + get_children_expression<children...>();
                }
                else
                {
                    return self + quantifier + "(?:"
                           + get_children_expression<children...>()
                           + ')';
                }
            }();

            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admitted_first_chars = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename matcher::admitted_first_chars>;

            // If this node has no intersections with its children,
            // backtracking is not needed
            template<typename... injected_children>
            using optimize = std::conditional_t<
                admitted_sets_intersection_t<
                    admitted_first_chars,
                    typename extract_admission_set<children..., injected_children...>::type>::empty,
                possessive<typename matcher::template optimize<>,
                           repetitions_min,
                           repetitions_max,
                           typename children::template optimize<>...>,
                greedy<typename matcher::template optimize<>,
                       repetitions_min,
                       repetitions_max,
                       typename children::template optimize<>...>>;

            template<typename... injected_children>
            static constexpr auto backtrack(auto result)
            {
                auto dfs_result = dfs<children...>(result);

                if (dfs_result)
                {
                    dfs_result.accepted
                        = dfs<injected_children...>(result).accepted;
                }

                return dfs_result;
            }

            template<typename... injected_children>
            static constexpr auto match(auto result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<children...>(result);
                }
                else
                {
                    for (std::size_t i = 0; i < repetitions_min; ++i)
                    {
                        if (result.actual_iterator_end
                            > result.query.end())
                        {
                            result = false;
                            return result;
                        }

                        result = matcher::template match<children...>(
                            result);

                        if (!result)
                        {
                            return result;
                        }
                    }

                    // Iterate until this matcher accepts
                    auto last_result = result;
                    auto repetitions = repetitions_min;

                    for (std::size_t i = repetitions_min;
                         i < repetitions_max;
                         ++i)
                    {
                        if (result.actual_iterator_end
                            >= result.query.end())
                        {
                            break;
                        }

                        result = matcher::template match<children...>(
                            result);

                        if (!result)
                        {
                            break;
                        }

                        last_result = result;
                        repetitions++;
                    }

                    // Now backtrack, if needed
                    if (auto res
                        = backtrack<injected_children...>(last_result);
                        res)
                    {
                        return res;
                    }

                    for (std::size_t i = repetitions; i > repetitions_min;
                         --i)
                    {
                        last_result.actual_iterator_end--;

                        if (matcher::template match<children...>(
                                last_result))
                        {
                            if (auto res
                                = backtrack<injected_children...>(
                                    last_result);
                                res)
                            {
                                return res;
                            }
                        }
                    }

                    last_result.accepted = false;
                    return last_result;
                }
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_GREEDY_HPP_*/
