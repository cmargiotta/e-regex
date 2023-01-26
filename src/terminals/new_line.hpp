#ifndef TERMINALS_NEW_LINE
#define TERMINALS_NEW_LINE

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex
{
    template<>
    struct terminal<pack_string<'\\', 'n'>> : public exact_matcher<pack_string<'\n'>>
    {
    };
}// namespace e_regex

#endif /* TERMINALS_NEW_LINE */