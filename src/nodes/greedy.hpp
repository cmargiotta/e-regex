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
#include "utilities/math.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             unsigned repetitions_min,
             unsigned repetitions_max = std::numeric_limits<unsigned>::max(),
             typename... children>
    struct greedy : public base<matcher, children...>
    {
            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admission_set = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename decltype(matcher::meta)::admission_set>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = e_regex::policy::GREEDY,
                .minimum_match_size
                = matcher::meta.minimum_match_size * repetitions_min,
                .maximum_match_size
                = repetitions_max == std::numeric_limits<unsigned>::max()
                      ? std::numeric_limits<unsigned>::max()
                      : matcher::meta.maximum_match_size * repetitions_max,
            };

            static constexpr unsigned groups
                = group_getter<matcher>::value
                  + sum(group_getter<children>::value...);

            static constexpr auto expression = []() {
                auto quantifier = []() {
                    if constexpr (repetitions_min == 0
                                  && repetitions_max
                                         == std::numeric_limits<unsigned>::max())
                    {
                        return static_string {"*"};
                    }
                    else if constexpr (repetitions_min == 1
                                       && repetitions_max
                                              == std::numeric_limits<
                                                  unsigned>::max())
                    {
                        return static_string {"+"};
                    }
                    else if constexpr (repetitions_min == 0
                                       && repetitions_max == 1)
                    {
                        return static_string {"?"};
                    }
                    else if constexpr (repetitions_max
                                       == std::numeric_limits<unsigned>::max())
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

            // If this node has no intersections with its children,
            // backtracking is not needed
            template<typename... injected_children>
            using optimize = std::conditional_t<
                admitted_sets_intersection_t<
                    admission_set,
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
            static constexpr auto backtrack(auto& result) -> auto&
            {
                if constexpr (sizeof...(children) > 0)
                {
                    return dfs<std::tuple<children...>>(result);
                }
                else
                {
                    if (result)
                    {
                        auto bak = result.actual_iterator_end;

                        dfs<std::tuple<injected_children...>>(result);
                        result.actual_iterator_end = bak;
                    }

                    return result;
                }
            }

            template<typename... injected_children>
            static constexpr auto match(auto& result) -> auto&
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<std::tuple<children...>>(result);
                }
                else
                {
                    if constexpr (repetitions_min > 0)
                    {
                        for (unsigned i = 0; i < repetitions_min; ++i)
                        {
                            if (!matcher::match(result))
                            {
                                return result;
                            }
                        }
                    }

                    // Iterate while this matcher accepts
                    auto       repetitions = repetitions_min;
                    const auto start     = result.actual_iterator_end;
                    auto       last_iter = start;

                    while (repetitions < repetitions_max)
                    {
                        if (!matcher::match(result))
                        {
                            // Restore last good match
                            result.actual_iterator_end = last_iter;
                            result                     = true;
                            break;
                        }

                        last_iter = result.actual_iterator_end;
                        repetitions++;
                    }

                    // Now backtrack
                    while (repetitions >= repetitions_min)
                    {
                        if (backtrack<injected_children...>(result))
                        {
                            return result;
                        }

                        for (; last_iter >= start; --last_iter)
                        {
                            result.actual_iterator_end = last_iter;

                            if (matcher::match(result))
                            {
                                break;
                            }
                        }

                        if (!result || repetitions == repetitions_min)
                        {
                            result.actual_iterator_end = last_iter;
                            break;
                        }

                        --repetitions;
                    }

                    if (repetitions >= repetitions_min)
                    {
                        return dfs<std::tuple<children...>>(result);
                    }
                    result = repetitions >= repetitions_min;
                    return result;
                }
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_GREEDY_HPP_*/
