#ifndef E_REGEX_NODES_REPEATED_HPP_
#define E_REGEX_NODES_REPEATED_HPP_

#include <cstddef>

#include "basic.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher, std::size_t repetitions, typename... children>
    struct repeated : public base<matcher, children...>
    {
            static constexpr auto expression = []() {
                auto self = matcher::expression + '{'
                            + number_to_pack_string_t<repetitions>::string
                            + '}';

                if constexpr (sizeof...(children) <= 1)
                {
                    return self + get_children_expression<children...>();
                }
                else
                {
                    return self + "(?:"
                           + get_children_expression<children...>()
                           + ')';
                }
            }();

            using admitted_first_chars =
                typename matcher::admitted_first_chars;

            template<typename... injected_children>
            using optimize
                = repeated<typename matcher::template optimize<>,
                           repetitions,
                           typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr auto match(auto& res) -> auto&
            {
                for (std::size_t i = 0; i < repetitions; ++i)
                {
                    if (!matcher::match(res))
                    {
                        return res;
                    }
                }

                return dfs<std::tuple<children...>>(res);
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_REPEATED_HPP_*/
