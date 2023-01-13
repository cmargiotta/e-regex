#ifndef TREE_BUILDER
#define TREE_BUILDER

#include <operators/operators.hpp>
#include <utilities/split.hpp>

namespace e_regex
{
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
