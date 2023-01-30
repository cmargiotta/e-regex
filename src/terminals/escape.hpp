#ifndef TERMINALS_ESCAPE
#define TERMINALS_ESCAPE

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'e'>> : public exact_matcher<pack_string<0x1B>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ESCAPE */
