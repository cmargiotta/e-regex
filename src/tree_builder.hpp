#ifndef TREE_BUILDER
#define TREE_BUILDER

#include <tuple>

#include "heuristics.hpp"
#include "matcher.hpp"
#include "static_string.hpp"
#include "terminal.hpp"
#include "tokenizer.hpp"
#include "utility.hpp"

namespace e_regex
{
    template<typename last_node, typename tokens>
    struct square_bracker_tree_builder_helper;

    template<typename last_node>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<>>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node = add_child_t<last_node, basic_node<exact_matcher<head>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, char identifier, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<pack_string<'\\', identifier>, tail...>>
    {
            // Simple case, iterate

            using new_node
                = add_child_t<last_node, basic_node<terminal<pack_string<'\\', identifier>>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename start, typename end, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<start, pack_string<'-'>, end, tail...>>
    {
            // Range found

            using new_node
                = add_child_t<last_node, basic_node<range_terminal<start, end>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename tokens>
    struct tree_builder_helper;

    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<>>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, tail...>>
    {
            // ? operator found
            using new_node = basic_node<optional_node<last_node>>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, tail...>>
    {
            // * operator found
            using new_node = basic_node<optional_node<repeated_node<last_node>>>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, tail...>>
    {
            // + operator found
            using new_node = basic_node<repeated_node<last_node>>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'('>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<grouping_node<subregex>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node,
                               std::tuple<pack_string<'('>, pack_string<'?'>, pack_string<':'>, tail...>>
    {
            // Non capturing group found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<subregex, std::tuple<>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<basic_node<void, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<subregex, std::tuple<>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, pack_string<'^'>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<basic_node<void, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<basic_node<negated_node<subregex>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename head, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node = typename tree_builder_helper<basic_node<terminal<head>, std::tuple<>>,
                                                          std::tuple<tail...>>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename tokens>
    struct tree_builder_branches;

    template<typename... subregexes>
    struct tree_builder_branches<std::tuple<subregexes...>>
    {
            using branches = std::tuple<typename tree_builder_helper<void, subregexes>::tree...>;
            using tree     = basic_node<void, branches>;
    };

    template<typename regex>
    struct tree_builder;

    template<char... regex>
    struct tree_builder<pack_string<regex...>>
        : public tree_builder_branches<split_t<'|', tokenize<pack_string<regex...>>>>
    {
    };

}// namespace e_regex

#endif /* TREE_BUILDER */
