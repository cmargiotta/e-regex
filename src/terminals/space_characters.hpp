#ifndef TERMINALS_SPACE_CHARACTERS
#define TERMINALS_SPACE_CHARACTERS

#include <algorithm>
#include <array>

#include "common.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 's'>> : public terminal_common<terminal<pack_string<'\\', 's'>>>
    {
            static constexpr auto match_(auto result)
            {
                constexpr std::array matched {' ', '\t', '\n', '\r', '\f'};

                result = std::find(matched.begin(), matched.end(), *result.actual_iterator_end)
                         != matched.end();

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'S'>> : public negated_terminal<terminal<pack_string<'\\', 's'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_SPACE_CHARACTERS */
