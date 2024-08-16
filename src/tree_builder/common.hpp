#ifndef HEURISTICS_COMMON_HPP
#define HEURISTICS_COMMON_HPP

#include <tuple>

#include "nodes.hpp"
#include "nodes/repeated.hpp"

namespace e_regex
{
    template<typename node, typename child>
    struct add_child;

    template<template<typename, typename...> typename matcher, typename match, typename... children, typename child>
    struct add_child<matcher<match, children...>, child>
    {
            using type = matcher<match, children..., child>;
    };

    template<template<typename, auto, auto, typename...> typename quantified_matcher,
             auto min,
             auto max,
             typename match,
             typename... children,
             typename child>
    struct add_child<quantified_matcher<match, min, max, children...>, child>
    {
            using type = quantified_matcher<match, min, max, children..., child>;
    };

    template<template<typename, auto, typename...> typename quantified_matcher,
             auto data,
             typename match,
             typename... children,
             typename child>
    struct add_child<quantified_matcher<match, data, children...>, child>
    {
            using type = quantified_matcher<match, data, children..., child>;
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
