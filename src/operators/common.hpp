#ifndef OPERATORS_COMMON
#define OPERATORS_COMMON

#include <tuple>

#include "heuristics.hpp"
#include "static_string.hpp"
#include "terminals/terminal.hpp"
#include "tokenizer.hpp"

namespace e_regex
{
    template<typename last_node, typename tokens>
    struct tree_builder_helper;

    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<>>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node =
                typename tree_builder_helper<basic_node<terminals::terminal<head>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename matcher>
    struct negated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                result = matcher::match(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };
}// namespace e_regex

#endif /* OPERATORS_COMMON */
