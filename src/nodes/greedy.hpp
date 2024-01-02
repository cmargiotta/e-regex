#ifndef NODES_GREEDY_HPP
#define NODES_GREEDY_HPP

#include <cstddef>
#include <limits>

#include "basic.hpp"
#include "utilities/literal_string_view.hpp"
#include "utilities/sum.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct greedy : public base<matcher, children...>
    {
            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(children::groups...);

            template<typename... second_layer_children>
            static constexpr auto recursive_match(auto result, std::size_t matches = 0)
            {
                if (result.actual_iterator_end >= result.query.end() || matches >= repetitions_max)
                {
                    result.accepted = (repetitions_min == 0 && sizeof...(children) == 0)
                                      && dfs<second_layer_children...>(result);
                    return result;
                }

                auto last_result = result;
                result           = matcher::match(result);

                if (result)
                {
                    if (auto res = recursive_match<second_layer_children...>(result, matches + 1); res)
                    {
                        // Recursion had success
                        return res;
                    }

                    // Recursion had no success, this iteration could be the acceptant one
                    result.accepted = (matches + 1) >= repetitions_min;

                    result          = dfs<children...>(result);
                    result.accepted = dfs<second_layer_children...>(result);
                    return result;
                }

                if constexpr (repetitions_min == 0)
                {
                    last_result          = dfs<children...>(last_result);
                    last_result.accepted = dfs<second_layer_children...>(last_result);
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
                    return recursive_match<second_layer_children...>(std::move(result));
                }
            }
    };
}// namespace e_regex::nodes

#endif /* NODES_GREEDY_HPP */
