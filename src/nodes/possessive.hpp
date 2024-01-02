#ifndef NODES_POSSESSIVE_HPP
#define NODES_POSSESSIVE_HPP

#include <limits>

#include "basic.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct possessive : public base<matcher, children...>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                for (auto i = 0; i < repetitions_max; ++i)
                {
                    auto last_result = matcher::template match<second_layer_children...>(result);

                    if (last_result)
                    {
                        result = last_result;
                    }
                    else
                    {
                        if (i < repetitions_min)
                        {
                            // Failed too early
                            return last_result;
                        }

                        // Failed but repetitions_min was satisfied, run dfs
                        break;
                    }
                }

                return dfs<children...>(result);
            }
    };
}// namespace e_regex::nodes

#endif /* NODES_POSSESSIVE_HPP */
