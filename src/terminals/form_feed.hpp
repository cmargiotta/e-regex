#ifndef E_REGEX_NODES_TERMINALS_FORM_FEED_HPP_
#define E_REGEX_NODES_TERMINALS_FORM_FEED_HPP_

#include "common.hpp"
#include "exact_matcher.hpp"

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'f'>>
        : public exact_matcher<pack_string<0x0C>>
    {
            static constexpr auto expression = static_string {"\\f"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_NODES_TERMINALS_FORM_FEED_HPP_*/
