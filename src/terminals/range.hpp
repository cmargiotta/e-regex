#ifndef TERMINALS_RANGE_HPP
#define TERMINALS_RANGE_HPP

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<pack_string<start>, pack_string<end>>
        : public terminal_common<range_terminal<pack_string<start>, pack_string<end>>>
    {
            using expression           = pack_string<'[', start, '-', end, ']'>;
            using admitted_first_chars = admitted_set_range_t<char, start, end>;

            static constexpr auto match(auto result)
            {
                static_assert(end >= start, "Range [a-b] must respect b >= a");

                auto current = result.actual_iterator_end;

                result = *current >= start && *current <= end;
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_RANGE_HPP */
