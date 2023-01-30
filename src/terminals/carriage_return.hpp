#ifndef TERMINALS_CARRIAGE_RETURN
#define TERMINALS_CARRIAGE_RETURN

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'r'>> : public exact_matcher<pack_string<'\r'>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_CARRIAGE_RETURN */
