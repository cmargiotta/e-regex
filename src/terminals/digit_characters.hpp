#ifndef E_REGEX_TERMINALS_DIGIT_CHARACTERS_HPP_
#define E_REGEX_TERMINALS_DIGIT_CHARACTERS_HPP_

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'd'>>
        : public terminal_common<terminal<pack_string<'\\', 'd'>>,
                                 admitted_set_range_t<char, '0', '9'>>
    {
            static constexpr auto expression = static_string {"\\d"};

            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                auto current = result.actual_iterator_end;

                result = (*current >= '0' && *current <= '9');
                result.actual_iterator_end++;

                return result;
            }
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
