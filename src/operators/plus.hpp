#ifndef OPERATORS_PLUS
#define OPERATORS_PLUS

#include "common.hpp"

namespace e_regex
{
    template<typename matcher>
    struct repeated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                unsigned matches = 0;

                while (result.actual_iterator_end < result.query.end())
                {
                    auto res = matcher::match(result);

                    if (res)
                    {
                        result = std::move(res);
                        matches++;
                    }
                    else
                    {
                        break;
                    }
                }

                result = matches > 0;

                return result;
            }
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, tail...>>
    {
            // + operator found
            using new_node = basic_node<repeated_node<last_node>>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_PLUS */
