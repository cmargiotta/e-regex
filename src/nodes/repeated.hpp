#ifndef NODES_REPEATED_HPP
#define NODES_REPEATED_HPP

#include <cstddef>

#include "basic.hpp"

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
}// namespace e_regex::nodes

#endif /* NODES_REPEATED_HPP */
