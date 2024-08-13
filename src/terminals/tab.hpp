#ifndef E_REGEX_NODES_TERMINALS_TAB_HPP_
#define E_REGEX_NODES_TERMINALS_TAB_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 't'>>
        : public exact_matcher<pack_string<'\t'>>
    {
            static constexpr auto expression = static_string {"\\t"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_NODES_TERMINALS_TAB_HPP_*/
