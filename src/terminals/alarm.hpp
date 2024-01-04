#ifndef TERMINALS_ALARM_HPP
#define TERMINALS_ALARM_HPP

#include "common.hpp"
#include "exact_matcher.hpp"
#include "static_string.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'a'>> : public exact_matcher<pack_string<0x07>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ALARM_HPP */
