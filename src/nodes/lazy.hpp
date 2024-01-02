#ifndef OPERATORS_LAZY_NODE_HPP
#define OPERATORS_LAZY_NODE_HPP

#include <limits>

#include "basic.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             std::size_t repetitions_min,
             std::size_t repetitions_max = std::numeric_limits<std::size_t>::max(),
             typename... children>
    struct lazy : public base<matcher, children...>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return dfs<children...>(result);
                }
                else
                {
                    auto matches = 0;

                    for (auto i = 0; i < repetitions_min; ++i)
                    {
                        result = matcher::template match<second_layer_children...>(result);
                        matches++;

                        if (!result)
                        {
                            return result;
                        }
                    }

                    while (result.actual_iterator_end < result.query.end() && matches < repetitions_max)
                    {
                        auto dfs_result = dfs<children...>(result);

                        if (!dfs_result)
                        {
                            result = matcher::template match<second_layer_children...>(result);
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

    template<typename matcher, typename... children>
    struct get_expression<lazy<matcher, 0, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self
                = merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'*', '?'>>;
            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, typename... children>
    struct get_expression<lazy<matcher, 1, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self
                = merge_pack_strings_t<typename get_expression_base<matcher>::type, pack_string<'+', '?'>>;
            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, auto min, typename... children>
    struct get_expression<lazy<matcher, min, std::numeric_limits<std::size_t>::max(), children...>>
    {
            using self = concatenate_pack_strings_t<pack_string<>,
                                                    typename get_expression_base<matcher>::type,
                                                    pack_string<'{'>,
                                                    number_to_pack_string_t<min>,
                                                    pack_string<',', '}', '?'>>;

            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };

    template<typename matcher, auto min, auto max, typename... children>
    struct get_expression<lazy<matcher, min, max, children...>>
    {
            using self = concatenate_pack_strings_t<pack_string<>,
                                                    typename get_expression_base<matcher>::type,
                                                    pack_string<'{'>,
                                                    number_to_pack_string_t<min>,
                                                    pack_string<','>,
                                                    number_to_pack_string_t<max>,
                                                    pack_string<'}', '?'>>;

            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };
}// namespace e_regex::nodes

#endif /* OPERATORS_LAZY_NODE_HPP */
