#ifndef HEURISTICS_COMMON_HPP
#define HEURISTICS_COMMON_HPP

#include <tuple>

#include <nodes.hpp>

namespace e_regex
{
    template<typename node, typename child>
    struct add_child;

    template<typename matcher,
             typename... children,
             std::size_t   repetitions_min,
             std::size_t   repetitions_max,
             nodes::policy repetition_policy,
             bool          grouping,
             typename child>
    struct add_child<
        nodes::basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, repetition_policy, grouping>,
        child>
    {
            using type
                = nodes::basic_node<matcher, std::tuple<children..., child>, repetitions_min, repetitions_max, repetition_policy, grouping>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;
}// namespace e_regex

#endif /* HEURISTICS_COMMON_HPP */
