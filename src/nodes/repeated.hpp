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
            using optimize = repeated;

            template<typename... injected_children>
            static constexpr auto match(auto res)
            {
                for (std::size_t i = 0; i < repetitions; ++i)
                {
                    if (res.actual_iterator_end >= res.query.end())
                    {
                        res = false;
                        return res;
                    }

                    res = matcher::template match<injected_children...>(
                        res);

                    if (!res)
                    {
                        return res;
                    }
                }

                return dfs<children...>(res);
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_REPEATED_HPP_*/
