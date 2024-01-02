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

    template<typename matcher, typename... children>
    struct get_expression<possessive<matcher, 0, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self
                = merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'*', '+'>>;
            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, typename... children>
    struct get_expression<possessive<matcher, 1, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self
                = merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'+'>>;
            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, auto min, typename... children>
    struct get_expression<possessive<matcher, min, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self = concatenate_pack_strings_t<pack_string<>,
                                                    typename get_expression_base<matcher>::type,
                                                    pack_string<'{'>,
                                                    number_to_pack_string_t<min>,
                                                    pack_string<',', '}', '+'>>;

            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, auto min, auto max, typename... children>
    struct get_expression<possessive<matcher, min, max, children...>>
    {
            using self = concatenate_pack_strings_t<pack_string<>,
                                                    typename get_expression_base<matcher>::type,
                                                    pack_string<'{'>,
                                                    number_to_pack_string_t<min>,
                                                    pack_string<','>,
                                                    number_to_pack_string_t<max>,
                                                    pack_string<'}', '+'>>;

            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };
}// namespace e_regex::nodes

#endif /* NODES_POSSESSIVE_HPP */
