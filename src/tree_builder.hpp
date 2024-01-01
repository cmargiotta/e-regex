#ifndef TREE_BUILDER_HPP
#define TREE_BUILDER_HPP

#include "operators.hpp"
#include "tokenizer.hpp"
#include "utilities/split.hpp"

namespace e_regex
{
    template<typename tokens>
    struct tree_builder_branches;

    template<typename... subregexes>
    struct tree_builder_branches<std::tuple<subregexes...>>
    {
            using branches = std::tuple<typename tree_builder_helper<void, subregexes>::tree...>;
            using tree     = nodes::basic<void, branches>;
    };

    template<typename regex>
    struct tree_builder;

    template<char... regex>
    struct tree_builder<pack_string<regex...>>
        : public tree_builder_branches<split_t<'|', tokenizer_t<pack_string<regex...>>>>
    {
    };

}// namespace e_regex

#endif /* TREE_BUILDER_HPP */
