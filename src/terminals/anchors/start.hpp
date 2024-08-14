#ifndef E_REGEX_TERMINALS_ANCHORS_START_HPP_
#define E_REGEX_TERMINALS_ANCHORS_START_HPP_

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct start
    {
            static constexpr auto expression = static_string {"^"};
            using admitted_first_chars       = admitted_set<char>;

            template<typename... injected_children>
            using optimize = start;

            template<typename... injected_children>
            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end
                          == result.query.begin());

                return result;
            }
    };
} // namespace e_regex::terminals::anchors

#endif /* E_REGEX_TERMINALS_ANCHORS_START_HPP_*/
