#ifndef E_REGEX_TERMINALS_ALARM_HPP_
#define E_REGEX_TERMINALS_ALARM_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'a'>>
        : public exact_matcher<pack_string<0x07>>
    {
            static constexpr auto expression = static_string {"\\a"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_ALARM_HPP_*/
