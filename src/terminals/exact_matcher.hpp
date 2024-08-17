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
              exact_matcher<pack_string<identifier, identifiers...>>,
              admitted_set<char, identifier, identifiers...>>
    {
            static constexpr auto expression
                = pack_string<identifier, identifiers...>::string;

            static constexpr auto meta
                = e_regex::meta<admitted_set<char, identifier, identifiers...>> {
                    .policy_            = e_regex::policy::EXACT,
                    .minimum_match_size = sizeof...(identifiers) + 1,
                    .maximum_match_size = sizeof...(identifiers) + 1,
                };

            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                auto start = result.actual_iterator_end;

                for (const auto& c:
                     pack_string<identifier, identifiers...>::string.to_view())
                {
                    result = c == *result.actual_iterator_end;
                    result.actual_iterator_end++;

                    if (!result)
                    {
                        result.actual_iterator_end = start;
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
