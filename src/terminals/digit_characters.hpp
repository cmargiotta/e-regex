#ifndef TERMINALS_DIGIT_CHARACTERS_HPP
#define TERMINALS_DIGIT_CHARACTERS_HPP

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'd'>> : public terminal_common<terminal<pack_string<'\\', 'd'>>>
    {
            using admitted_first_chars = admitted_set_range_t<char, '0', '9'>;

            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= '0' && *current <= '9');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'D'>> : public negated_terminal<terminal<pack_string<'\\', 'd'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_DIGIT_CHARACTERS_HPP */
