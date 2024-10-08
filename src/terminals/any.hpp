#ifndef E_REGEX_TERMINALS_ANY_HPP_
#define E_REGEX_TERMINALS_ANY_HPP_

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'.'>>
        : public terminal_common<terminal<pack_string<'.'>>,
                                 admitted_set_range_t<char, '\0', '\x7F'>>
    {
            static constexpr auto expression = static_string {"."};

            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                result = *result.actual_iterator_end != '\n';

                if (result)
                {
                    result.actual_iterator_end++;
                }

                return result;
            }
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_ANY_HPP_*/
