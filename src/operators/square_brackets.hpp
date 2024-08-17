#ifndef E_REGEX_OPERATORS_SQUARE_BRACKETS_HPP_
#define E_REGEX_OPERATORS_SQUARE_BRACKETS_HPP_

#include "common.hpp"
#include "terminals/exact_matcher.hpp"
#include "terminals/range.hpp"
#include "utilities/extract_delimited_content.hpp"

namespace e_regex
{
    namespace __private
    {
        template<typename node, typename child>
        struct add_branch;

        template<typename identifier>
        struct add_branch<nodes::simple<void>, terminals::exact_matcher<identifier>>
        {
                using type = terminals::exact_matcher<identifier>;
        };

        template<template<typename, typename...> typename matcher,
                 typename... children,
                 typename match,
                 typename child>
        struct add_branch<matcher<match, children...>, child>
        {
                using type = matcher<match, children..., child>;
        };

        template<typename node, typename child>
        using add_branch_t = typename add_branch<node, child>::type;
    } // namespace __private

    template<typename last_node, typename tokens, auto group_index>
    struct square_bracker_tree_builder_helper;

    template<typename last_node, auto group_index>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<>, group_index>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail, auto group_index>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<head, tail...>, group_index>
    {
            // Simple case, iterate

            using new_node = __private::add_branch_t<
                last_node,
                nodes::simple<terminals::exact_matcher<head>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node,
                                                            std::tuple<tail...>,
                                                            group_index>::tree;
    };

    template<typename last_node, char identifier, typename... tail, auto group_index>
    struct square_bracker_tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', identifier>, tail...>,
        group_index>
    {
            // Simple case, iterate

            using new_node = __private::add_branch_t<
                last_node,
                nodes::simple<terminals::terminal<pack_string<'\\', identifier>>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node,
                                                            std::tuple<tail...>,
                                                            group_index>::tree;
    };

    template<typename last_node, typename start, typename end, typename... tail, auto group_index>
    struct square_bracker_tree_builder_helper<
        last_node,
        std::tuple<start, pack_string<'-'>, end, tail...>,
        group_index>
    {
            // Range found

            using new_node = __private::add_branch_t<
                last_node,
                nodes::simple<terminals::range_terminal<start, end>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node,
                                                            std::tuple<tail...>,
                                                            group_index>::tree;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, tail...>, group_index>
    {
            // [ found
            using substring
                = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex
                = nodes::simple<typename square_bracker_tree_builder_helper<
                    nodes::simple<void>,
                    typename substring::result,
                    group_index>::tree>;

            using new_node =
                typename tree_builder_helper<subregex,
                                             typename substring::remaining,
                                             subregex::next_group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'['>, pack_string<'^'>, tail...>,
        group_index>
    {
            // [ found
            using substring
                = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex = typename square_bracker_tree_builder_helper<
                nodes::simple<void>,
                typename substring::result,
                group_index>::tree;

            // if subregex_group_index is 0 the subregex does not
            // contain any group

            using new_node = typename tree_builder_helper<
                nodes::simple<nodes::negated_node<subregex>>,
                typename substring::remaining,
                subregex::next_group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

} // namespace e_regex

#endif /* E_REGEX_OPERATORS_SQUARE_BRACKETS_HPP_*/
