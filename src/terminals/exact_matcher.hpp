#ifndef TERMINALS_EXACT_MATCHER
#define TERMINALS_EXACT_MATCHER

#include "common.hpp"
#include "static_string.hpp"

namespace e_regex::terminals
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

    template<char... identifier>
    struct exact_matcher<pack_string<identifier...>>
        : public terminal_common<exact_matcher<pack_string<identifier...>>>
    {
            static constexpr auto match_(auto result)
            {
                for (auto c: pack_string<identifier...>::string)
                {
                    result = c == *result.actual_iterator_end;
                    result.actual_iterator_end++;

                    if (!result)
                    {
                        break;
                    }
                }

                return result;
            }
    };

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_EXACT_MATCHER */
