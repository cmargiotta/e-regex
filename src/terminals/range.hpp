#ifndef TERMINALS_RANGE
#define TERMINALS_RANGE

#include "common.hpp"

namespace e_regex
{
    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<pack_string<start>, pack_string<end>>
        : public terminal_common<range_terminal<pack_string<start>, pack_string<end>>>
    {
            static constexpr auto match(auto result)
            {
                static_assert(end >= start, "Range [a-b] must respect b >= a");

                auto current = result.actual_iterator_end;

                result = *current >= start && *current <= end;
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex

#endif /* TERMINALS_RANGE */
