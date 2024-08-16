#ifndef E_REGEX_TERMINALS_EXACT_MATCHER_HPP_
#define E_REGEX_TERMINALS_EXACT_MATCHER_HPP_

#include "common.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/static_string.hpp"

namespace e_regex::terminals
{
    template<typename identifier>
    struct exact_matcher;

    template<char identifier, char... identifiers>
    struct exact_matcher<pack_string<identifier, identifiers...>>
        : public terminal_common<
              exact_matcher<pack_string<identifier, identifiers...>>>
    {
            static constexpr auto expression
                = pack_string<identifier, identifiers...>::string;
            using admitted_first_chars = admitted_set<char, identifier>;

            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                for (const auto& c:
                     pack_string<identifier, identifiers...>::string.to_view())
                {
                    result = c == *result.actual_iterator_end;
                    result.actual_iterator_end++;

                    if (!result)
                    {
                        return result;
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
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_EXACT_MATCHER_HPP_*/
