#ifndef OPERATORS_STAR
#define OPERATORS_STAR

#include "braces.hpp"
#include "common.hpp"
#include "optional.hpp"

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, tail...>>
    {
            // greedy * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'?'>, tail...>>
    {
            // lazy * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'+'>, tail...>>
    {
            // possessive * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_STAR */
