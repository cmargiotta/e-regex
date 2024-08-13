#ifndef E_REGEX_TREE_BUILDER_HPP_
#define E_REGEX_TREE_BUILDER_HPP_

#include "nodes.hpp"
#include "operators.hpp"
#include "tokenizer.hpp"
#include "utilities/split.hpp"

namespace e_regex
{
    template<typename parsed, typename branches, auto group_index>
    struct tree_builder_branches;

    template<typename... parsed, typename subregex, typename... subregexes, auto group_index>
    struct tree_builder_branches<std::tuple<parsed...>,
                                 std::tuple<subregex, subregexes...>,
                                 group_index>
    {
            using subtree =
                typename tree_builder_helper<void, subregex, group_index>::tree;

            using tree =
                typename tree_builder_branches<std::tuple<parsed..., subtree>,
                                               std::tuple<subregexes...>,
                                               subtree::next_group_index>::tree;
    };

    template<typename... parsed, auto group_index>
    struct tree_builder_branches<std::tuple<parsed...>, std::tuple<>, group_index>
    {
            using tree = nodes::simple<void, parsed...>;
    };

    template<typename regex>
    struct tree_builder;

    template<char... regex>
    struct tree_builder<pack_string<regex...>>
        : public tree_builder_branches<
              std::tuple<>,
              split_t<'|', tokenizer_t<pack_string<regex...>>>,
              0>
    {};

} // namespace e_regex

#endif /* E_REGEX_TREE_BUILDER_HPP_*/
