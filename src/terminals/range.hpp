#ifndef E_REGEX_TERMINALS_RANGE_HPP_
#define E_REGEX_TERMINALS_RANGE_HPP_

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<pack_string<start>, pack_string<end>>
        : public terminal_common<
              range_terminal<pack_string<start>, pack_string<end>>,
              admitted_set_range_t<char, start, end>>
    {
            static constexpr auto expression
                = pack_string<'[', start, '-', end, ']'>::string;

            template<typename... injected_children>
            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                static_assert(end >= start,
                              "Range [a-b] must respect b >= a");

                const auto& current = *result.actual_iterator_end;

                result = current >= start && current <= end;
                if (result)
                {
                    result.actual_iterator_end++;
                }

                return result;
            }
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_RANGE_HPP_*/
