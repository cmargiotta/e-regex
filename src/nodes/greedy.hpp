#ifndef E_REGEX_NODES_GREEDY_HPP_
#define E_REGEX_NODES_GREEDY_HPP_

#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

#include "common.hpp"
#include "get_expression.hpp"
#include "nodes/possessive.hpp"
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
                auto self = matcher::expression + []() {
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
                               + static_string {","}
                               + number_to_pack_string_t<repetitions_max>::string
                               + static_string {"}"};
                    }
                }();

                return self + get_children_expression<children...>();
            }();

            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admitted_first_chars = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename matcher::admitted_first_chars>;

            template<typename... second_layer_children>
            static constexpr auto recursive_match(auto result,
                                                  std::size_t matches = 0)
            {
                if (result.actual_iterator_end >= result.query.end()
                    || matches >= repetitions_max)
                {
                    result.accepted
                        = (repetitions_min == 0
                           && sizeof...(children) == 0)
                          && dfs<second_layer_children...>(result);
                    return result;
                }

                auto last_result = result;
                result           = matcher::match(result);

                if (result)
                {
                    if (auto res
                        = recursive_match<second_layer_children...>(
                            result, matches + 1);
                        res)
                    {
                        // Recursion had success
                        return res;
                    }

                    // Recursion had no success, this iteration could
                    // be the acceptant one
                    result.accepted = (matches + 1) >= repetitions_min;

                    result = dfs<children...>(result);
                    result.accepted
                        = dfs<second_layer_children...>(result);
                    return result;
                }

                if constexpr (repetitions_min == 0)
                {
                    last_result = dfs<children...>(last_result);
                    last_result.accepted
                        = dfs<second_layer_children...>(last_result);
                    return last_result;
                }

                // Current iteration is not matching
                return result;
            }

            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<children...>(result);
                }
                else
                {
                    return recursive_match<second_layer_children...>(
                        std::move(result));
                }
            }
    };

    template<typename matcher, std::size_t repetitions_min, std::size_t repetitions_max, typename child>
        requires(admitted_sets_intersection_t<
                 typename matcher::admitted_first_chars,
                 typename child::admitted_first_chars>::empty)
    struct greedy<matcher, repetitions_min, repetitions_max, child>
        : public possessive<matcher, repetitions_min, repetitions_max, child>
    {};
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_GREEDY_HPP_*/
