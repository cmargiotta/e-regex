#ifndef TERMINALS_ANY_HPP
#define TERMINALS_ANY_HPP

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'.'>> : public terminal_common<terminal<pack_string<'.'>>>
    {
            using admitted_first_chars = admitted_set_range_t<char, '\0', '\x7F'>;

            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end != '\n';
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ANY_HPP */
