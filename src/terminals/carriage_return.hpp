#ifndef E_REGEX_NODES_TERMINALS_CARRIAGE_RETURN_HPP_
#define E_REGEX_NODES_TERMINALS_CARRIAGE_RETURN_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'r'>>
        : public exact_matcher<pack_string<'\r'>>
    {
            static constexpr auto expression = static_string {"\\r"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_NODES_TERMINALS_CARRIAGE_RETURN_HPP_*/
