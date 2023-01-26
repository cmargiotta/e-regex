#ifndef TERMINALS_ALARM
#define TERMINALS_ALARM

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex
{
    template<>
    struct terminal<pack_string<'\\', 'a'>> : public exact_matcher<pack_string<0x07>>
    {
    };
}// namespace e_regex

#endif /* TERMINALS_ALARM */
