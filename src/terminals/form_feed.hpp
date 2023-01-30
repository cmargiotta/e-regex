#ifndef TERMINALS_FORM_FEED
#define TERMINALS_FORM_FEED

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'f'>> : public exact_matcher<pack_string<0x0C>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_FORM_FEED */
