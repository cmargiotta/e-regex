#ifndef HEURISTICS
#define HEURISTICS

#include <tuple>

#include "matcher.hpp"
#include "static_string.hpp"
#include "terminals/terminal.hpp"

namespace e_regex
{
    template<typename node, typename child>
    struct add_child;

    template<typename matcher, typename... children, typename child>
    struct add_child<basic_node<matcher, std::tuple<children...>>, child>
    {
            using type = basic_node<matcher, std::tuple<children..., child>>;
    };

    template<typename... terminals, typename... terminals_to_add>
    struct add_child<basic_node<terminal<terminals...>, std::tuple<>>, basic_node<terminal<terminals_to_add...>>>
    {
            // Terminal sequence found, aggregate them
            using type = basic_node<terminal<terminals..., terminals_to_add...>, std::tuple<>>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;
}// namespace e_regex

#endif /* HEURISTICS */
