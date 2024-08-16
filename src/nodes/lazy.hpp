#ifndef E_REGEX_NODES_LAZY_HPP_
#define E_REGEX_NODES_LAZY_HPP_

#include <cstddef>
#include <limits>

#include "basic.hpp"
#include "meta.hpp"
#include "nodes/possessive.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             unsigned repetitions_min,
             unsigned repetitions_max = std::numeric_limits<unsigned>::max(),
             typename... children>
    struct lazy : public base<matcher, children...>
    {
            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admission_set = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename decltype(matcher::meta)::admission_set>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = e_regex::policy::LAZY,
                .minimum_match_size
                = matcher::meta.minimum_match_size * repetitions_min,
                .maximum_match_size
                = repetitions_max == std::numeric_limits<unsigned>::max()
                      ? std::numeric_limits<unsigned>::max()
                      : matcher::meta.maximum_match_size * repetitions_max,
            };

            static constexpr auto expression = []() {
                auto quantifier = []() {
                    if constexpr (repetitions_min == 0
                                  && repetitions_max
                                         == std::numeric_limits<unsigned>::max())
                    {
                        return static_string {"*?"};
                    }
                    else if constexpr (repetitions_min == 1
                                       && repetitions_max
                                              == std::numeric_limits<
                                                  unsigned>::max())
                    {
                        return static_string {"+?"};
                    }
                    else if constexpr (repetitions_min == 0
                                       && repetitions_max == 1)
                    {
                        return static_string {"??"};
                    }
                    else if constexpr (repetitions_max
                                       == std::numeric_limits<unsigned>::max())
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
                lazy<typename matcher::template optimize<>,
                     repetitions_min,
                     repetitions_max,
                     typename children::template optimize<>...>>;

            template<typename... injected_children>
            static constexpr auto match(auto& result) -> auto&
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<std::tuple<children...>>(result);
                }
                else
                {
                    for (unsigned i = 0; i < repetitions_min; ++i)
                    {
                        if (result.actual_iterator_end
                            >= result.query.end())
                        {
                            result = false;
                            return result;
                        }

                        if (!matcher::match(result))
                        {
                            return result;
                        }
                    }

                    unsigned matches = repetitions_min;

                    while (result.actual_iterator_end < result.query.end()
                           && matches < repetitions_max)
                    {
                        if (result.actual_iterator_end
                            >= result.query.end())
                        {
                            break;
                        }

                        auto dfs_result = result;

                        if constexpr (sizeof...(children) > 0)
                        {
                            dfs<std::tuple<children...>>(dfs_result);

                            if (dfs_result)
                            {
                                result = dfs_result;
                                return result;
                            }
                        }
                        else
                        {
                            dfs<std::tuple<injected_children...>>(
                                dfs_result);
                        }

                        if (!dfs_result)
                        {
                            matcher::match(result);
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

                    return dfs<std::tuple<children...>>(result);
                }
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_LAZY_HPP_*/
