#ifndef TREE_BUILDER
#define TREE_BUILDER

#include <tuple>

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

            using new_node
                = add_child_t<last_node, tree_node<terminal<head>, false, false, false, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename start, typename end, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<start, static_string<'-'>, end, tail...>>
    {
            // Range found

            using new_node
                = add_child_t<last_node,
                              tree_node<range_terminal<start, end>, false, false, false, std::tuple<>>>;
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
    struct tree_builder_helper<last_node, std::tuple<static_string<'*'>, tail...>>
    {
            // * operator found
            using new_node = set_repetition_t<set_optional_t<last_node, true>, true>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<static_string<'+'>, tail...>>
    {
            // + operator found
            using new_node = set_repetition_t<last_node, true>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<static_string<'('>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<tree_node<subregex, false, false, true, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<static_string<'['>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<tree_node<void, false, false, false, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<tree_node<subregex, false, false, false, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<static_string<'['>, static_string<'^'>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<tree_node<void, false, false, false, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node = typename tree_builder_helper<
                tree_node<negated_terminal<subregex>, false, false, false, std::tuple<>>,
                typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename head, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node =
                typename tree_builder_helper<tree_node<terminal<head>, false, false, false, std::tuple<>>,
                                             std::tuple<tail...>>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename regex>
    struct tree_builder;

    template<char... regex>
    struct tree_builder<static_string<regex...>>
        : public tree_builder_helper<void, tokenize<static_string<regex...>>>
    {
    };

}// namespace e_regex

#endif /* TREE_BUILDER */
