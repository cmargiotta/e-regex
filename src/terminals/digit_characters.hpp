#ifndef E_REGEX_TERMINALS_DIGIT_CHARACTERS_HPP_
#define E_REGEX_TERMINALS_DIGIT_CHARACTERS_HPP_

#include "common.hpp"
#include "terminals/range.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'd'>>
        : public range_terminal<pack_string<'0'>, pack_string<'9'>>
    {
            static constexpr auto expression = static_string {"\\d"};
    };

    template<>
    struct terminal<pack_string<'\\', 'D'>>
        : public negated_terminal<terminal<pack_string<'\\', 'd'>>,
                                  admitted_set_range_t<char, '0', '9'>>
    {
            static constexpr auto expression = static_string {"\\D"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_DIGIT_CHARACTERS_HPP_*/
