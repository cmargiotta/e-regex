#ifndef TERMINALS_EXACT_MATCHER_HPP
#define TERMINALS_EXACT_MATCHER_HPP

#include "common.hpp"
#include "static_string.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<typename identifier>
    struct exact_matcher;

    template<char identifier, char... identifiers>
    struct exact_matcher<pack_string<identifier, identifiers...>>
        : public terminal_common<exact_matcher<pack_string<identifier, identifiers...>>>
    {
            using admitted_first_chars = admitted_set<char, identifier>;

            static constexpr auto match_(auto result)
            {
                for (auto c: pack_string<identifier, identifiers...>::string.to_view())
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
            static constexpr bool exact = true;
    };

    template<char... chars>
    struct rebuild_expression<exact_matcher<pack_string<chars...>>>
    {
            using string = pack_string<chars...>;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_EXACT_MATCHER_HPP */
