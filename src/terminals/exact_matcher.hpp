#ifndef TERMINALS_EXACT_MATCHER
#define TERMINALS_EXACT_MATCHER

#include "common.hpp"

namespace e_regex
{
    template<typename identifier>
    struct exact_matcher;

    template<char identifier>
    struct exact_matcher<pack_string<identifier>>
        : public terminal_common<exact_matcher<pack_string<identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                result = identifier == *result.actual_iterator_end;
                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
    };
}// namespace e_regex

#endif /* TERMINALS_EXACT_MATCHER */