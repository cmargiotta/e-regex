#ifndef E_REGEX_NODES_TERMINALS_ESCAPE_HPP_
#define E_REGEX_NODES_TERMINALS_ESCAPE_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'e'>>
        : public exact_matcher<pack_string<0x1B>>
    {
            static constexpr auto expression = static_string {"\\e"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_NODES_TERMINALS_ESCAPE_HPP_*/
