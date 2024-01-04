#ifndef NODES_GET_EXPRESSION_HPP
#define NODES_GET_EXPRESSION_HPP

#include <cstddef>
#include <tuple>

#include "terminals.hpp"

namespace e_regex::nodes
{
    template<typename matcher, typename... children>
    struct get_expression_base;

    template<typename... terminals_>
    struct get_expression_base<terminals::terminal<terminals_...>>
    {
            using type =
                typename e_regex::terminals::rebuild_expression<e_regex::terminals::terminal<terminals_...>>::string;
    };

    template<typename terminal>
    struct get_expression_base<terminals::exact_matcher<terminal>>
    {
            using type =
                typename e_regex::terminals::rebuild_expression<e_regex::terminals::terminal<terminal>>::string;
    };

    template<typename matcher>
    struct get_expression_base<matcher>
    {
            using type = typename matcher::expression;
    };

    template<>
    struct get_expression_base<void>
    {
            using type = pack_string<>;
    };

    template<typename matcher, typename child, typename... children>
        requires(!std::same_as<matcher, void>)
    struct get_expression_base<matcher, child, children...>
    {
            using matcher_string = merge_pack_strings_t<typename get_expression_base<matcher>::type,
                                                        typename get_expression_base<child>::type>;

            using type = concatenate_pack_strings_t<pack_string<'|'>,
                                                    matcher_string,
                                                    typename get_expression_base<children>::type...>;
    };

    template<typename... children>
    struct get_expression_base<void, children...>
    {
            using type
                = concatenate_pack_strings_t<pack_string<'|'>, typename get_expression_base<children>::type...>;
    };
}// namespace e_regex::nodes

#endif /* NODES_GET_EXPRESSION_HPP */
