#ifndef NODES_POSSESSIVE_HPP
#define NODES_POSSESSIVE_HPP

#include <cstddef>
#include <limits>

#include "basic.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct possessive : public base<matcher, children...>
    {
            using self_expression = std::conditional_t<
                repetitions_min == 0 && repetitions_max == std::numeric_limits<std::size_t>::max(),
                merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'*', '+'>>,
                std::conditional_t<
                    repetitions_min == 1 && repetitions_max == std::numeric_limits<std::size_t>::max(),
                    merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'+', '+'>>,
                    std::conditional_t<repetitions_max == std::numeric_limits<std::size_t>::max(),
                                       concatenate_pack_strings_t<pack_string<>,
                                                                  typename get_expression_base<matcher>::type,
                                                                  pack_string<'{'>,
                                                                  number_to_pack_string_t<repetitions_min>,
                                                                  pack_string<',', '}', '+'>>,
                                       concatenate_pack_strings_t<pack_string<>,
                                                                  typename get_expression_base<matcher>::type,
                                                                  pack_string<'{'>,
                                                                  number_to_pack_string_t<repetitions_min>,
                                                                  pack_string<','>,
                                                                  number_to_pack_string_t<repetitions_max>,
                                                                  pack_string<'}', '+'>>>>>;
            using children_expression = typename get_expression_base<void, children...>::type;
            using expression          = merge_pack_strings_t<self_expression, children_expression>;

            // If matcher is optional (aka repetitions_min==0), admission set must include children
            // too
            using admitted_first_chars
                = std::conditional_t<repetitions_min == 0,
                                     typename extract_admission_set<matcher, children...>::type,
                                     typename matcher::admitted_first_chars>;

            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                for (std::size_t i = 0; i < repetitions_max; ++i)
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
