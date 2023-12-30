#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

#include <tuple>

#include "operators/basic_node.hpp"
#include "static_string.hpp"
#include "terminals/exact_matcher.hpp"
#include "terminals/terminal.hpp"

namespace e_regex
{
    template<typename node, typename child>
    struct add_child;

    template<typename matcher,
             typename... children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping,
             typename child>
    struct add_child<basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, repetition_policy, grouping>,
                     child>
    {
            using type
                = basic_node<matcher, std::tuple<children..., child>, repetitions_min, repetitions_max, repetition_policy, grouping>;
    };

    /*
        Regexes like "aaaaabc" become a single matcher with the whole string
    */
    template<char... identifiers, char... child_identifiers, typename... children, policy repetition_policy>
        requires terminals::terminal<pack_string<identifiers...>>::exact
                 && terminals::terminal<pack_string<child_identifiers...>>::exact
    struct add_child<
        basic_node<terminals::terminal<pack_string<identifiers...>>, std::tuple<>, 1, 1, repetition_policy, false>,
        basic_node<terminals::terminal<pack_string<child_identifiers...>>, std::tuple<children...>, 1, 1, repetition_policy, false>>
    {
            using type
                = basic_node<terminals::terminal<pack_string<identifiers..., child_identifiers...>>,
                             std::tuple<children...>,
                             1,
                             1,
                             repetition_policy,
                             false>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;
}// namespace e_regex

#endif /* HEURISTICS_HPP */
