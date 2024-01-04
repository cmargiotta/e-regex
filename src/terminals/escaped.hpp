#ifndef TERMINALS_ESCAPED
#define TERMINALS_ESCAPED

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<char identifier>
    struct terminal<pack_string<'\\', identifier>> : public exact_matcher<pack_string<identifier>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ESCAPED */
