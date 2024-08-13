#ifndef E_REGEX_NODES_TERMINALS_ESCAPED_HPP_
#define E_REGEX_NODES_TERMINALS_ESCAPED_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<char identifier>
    struct terminal<pack_string<'\\', identifier>>
        : public exact_matcher<pack_string<identifier>>
    {
            static constexpr auto expression
                = pack_string<'\\', identifier>::string;
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_NODES_TERMINALS_ESCAPED_HPP_*/
