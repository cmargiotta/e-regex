#ifndef TERMINALS_ALARM
#define TERMINALS_ALARM

#include "common.hpp"

namespace e_regex
{
    template<>
    struct terminal<pack_string<'\\', 'a'>> : public terminal_common<terminal<pack_string<'\\', 'a'>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end == 0x07;
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex

#endif /* TERMINALS_ALARM */
