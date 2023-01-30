#ifndef TERMINALS_ANY
#define TERMINALS_ANY

#include "common.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'.'>> : public terminal_common<terminal<pack_string<'.'>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end != '\n';
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ANY */
