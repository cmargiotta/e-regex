#ifndef TERMINALS_ESCAPED
#define TERMINALS_ESCAPED

#include "common.hpp"

namespace e_regex::terminals
{
    template<char identifier>
    struct terminal<pack_string<'\\', identifier>>
        : public terminal_common<terminal<pack_string<'\\', identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end == identifier;

                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ESCAPED */
