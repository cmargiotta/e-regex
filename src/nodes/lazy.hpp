#ifndef E_REGEX_NODES_LAZY_HPP_
#define E_REGEX_NODES_LAZY_HPP_

#include <cstddef>
#include <limits>

#include "basic.hpp"
#include "nodes/possessive.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max
             = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct lazy : public base<matcher, children...>
    {
            static constexpr auto expression = []() {
                auto quantifier = []() {
                    if constexpr (repetitions_min == 0
                                  && repetitions_max
                                         == std::numeric_limits<std::size_t>::max())
                    {
                        return static_string {"*?"};
                    }
                    else if constexpr (repetitions_min == 1
                                       && repetitions_max
                                              == std::numeric_limits<
                                                  std::size_t>::max())
                    {
                        return static_string {"+?"};
                    }
                    else if constexpr (repetitions_min == 0
                                       && repetitions_max == 1)
                    {
                        return static_string {"??"};
                    }
                    else if constexpr (repetitions_max
                                       == std::numeric_limits<std::size_t>::max())
                    {
                        return '{'
                               + number_to_pack_string_t<repetitions_min>::string
                               + ",}?";
                    }
                    else
                    {
                        return "{"
                               + number_to_pack_string_t<repetitions_min>::string
                               + ","
                               + number_to_pack_string_t<repetitions_max>::string
                               + "}?";
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
                lazy<typename matcher::template optimize<>,
                     repetitions_min,
                     repetitions_max,
                     typename children::template optimize<>...>>;

            template<typename... injected_children>
            static constexpr auto match(auto result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<children...>(result);
                }
                else
                {
                    std::size_t matches = 0;

                    for (std::size_t i = 0; i < repetitions_min; ++i)
                    {
                        if (result.actual_iterator_end
                            > result.query.end())
                        {
                            result = false;
                            return result;
                        }

                        result
                            = matcher::template match<injected_children...>(
                                result);
                        matches++;

                        if (!result)
                        {
                            return result;
                        }
                    }

                    while (result.actual_iterator_end < result.query.end()
                           && matches < repetitions_max)
                    {
                        auto dfs_result = dfs<children...>(result);

                        if constexpr (sizeof...(injected_children) > 0)
                        {
                            if (dfs_result)
                            {
                                dfs_result = dfs<injected_children...>(
                                    dfs_result);
                            }
                        }

                        if (!dfs_result)
                        {
                            result = matcher::template match<>(result);
                            matches++;

                            if (!result)
                            {
                                return result;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }

                    result.accepted = matches < repetitions_max;
                    return result;
                }
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_LAZY_HPP_*/
