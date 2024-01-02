#ifndef NODES_REPEATED_HPP
#define NODES_REPEATED_HPP

#include <cstddef>

#include "basic.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher, std::size_t repetitions, typename... children>
    struct repeated : public base<matcher, children...>
    {
            template<typename... second_layer_children>
            static constexpr auto match(auto res)
            {
                for (auto i = 0; i < repetitions; ++i)
                {
                    res = matcher::template match<second_layer_children...>(res);

                    if (!res)
                    {
                        return res;
                    }
                }

                return dfs<children...>(res);
            }
    };

    template<typename matcher, auto repetitions, typename... children>
    struct get_expression<repeated<matcher, repetitions, children...>>
    {
            using self = concatenate_pack_strings_t<pack_string<>,
                                                    typename get_expression_base<matcher>::type,
                                                    pack_string<'{'>,
                                                    number_to_pack_string_t<repetitions>,
                                                    pack_string<'}'>>;

            using children_ = typename get_expression_base<void, children...>::type;

            using type = merge_pack_strings_t<self, children_>;
    };
}// namespace e_regex::nodes

#endif /* NODES_REPEATED_HPP */
