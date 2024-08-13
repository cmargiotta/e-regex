#ifndef E_REGEX_NODES_LAZY_HPP_
#define E_REGEX_NODES_LAZY_HPP_

#include <cstddef>
#include <limits>

#include "basic.hpp"
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
                auto self = matcher::expression + []() {
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

                return self + get_children_expression<children...>();
            }();

            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admitted_first_chars = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename matcher::admitted_first_chars>;

            template<typename... second_layer_children>
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
                        result
                            = matcher::template match<second_layer_children...>(
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

                        if (!dfs_result)
                        {
                            result
                                = matcher::template match<second_layer_children...>(
                                    result);
                            matches++;

                            if (!result)
                            {
                                return result;
                            }
                        }
                        else
                        {
                            return dfs_result;
                        }
                    }

                    result.accepted = matches < repetitions_max;
                    return result;
                }
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_LAZY_HPP_*/
