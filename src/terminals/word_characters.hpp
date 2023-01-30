#ifndef TERMINALS_WORD_CHARACTERS
#define TERMINALS_WORD_CHARACTERS

#include "common.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'w'>> : public terminal_common<terminal<pack_string<'\\', 'w'>>>
    {
            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= 'A' && *current <= 'Z') || (*current >= 'a' && *current <= 'z');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'W'>> : public negated_terminal<terminal<pack_string<'\\', 'w'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_WORD_CHARACTERS */
