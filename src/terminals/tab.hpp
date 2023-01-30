#ifndef TERMINALS_TAB
#define TERMINALS_TAB

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 't'>> : public exact_matcher<pack_string<'\t'>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_TAB */
