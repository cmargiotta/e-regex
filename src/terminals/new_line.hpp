#ifndef E_REGEX_TERMINALS_NEW_LINE_HPP_
#define E_REGEX_TERMINALS_NEW_LINE_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'n'>>
        : public exact_matcher<pack_string<'\n'>>
    {
            static constexpr auto expression = static_string {"\\n"};
    };

    template<>
    struct terminal<pack_string<'\\', 'N'>>
        : public negated_terminal<terminal<pack_string<'\\', 'n'>>,
                                  admitted_set<char, '\n'>>
    {
            static constexpr auto expression = static_string {"\\N"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_NEW_LINE_HPP_*/
