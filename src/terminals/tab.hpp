#ifndef TERMINALS_TAB
#define TERMINALS_TAB

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex
{
    template<>
    struct terminal<pack_string<'\\', 't'>> : public exact_matcher<pack_string<'\t'>>
    {
    };
}// namespace e_regex

#endif /* TERMINALS_TAB */
