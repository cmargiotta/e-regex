#ifndef TERMINALS_DIGIT_CHARACTERS
#define TERMINALS_DIGIT_CHARACTERS

#include "common.hpp"

namespace e_regex
{
    template<>
    struct terminal<pack_string<'\\', 'd'>> : public terminal_common<terminal<pack_string<'\\', 'd'>>>
    {
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
}// namespace e_regex

#endif /* TERMINALS_DIGIT_CHARACTERS */
