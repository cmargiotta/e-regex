#ifndef E_REGEX_TERMINALS_ANCHORS_END_HPP_
#define E_REGEX_TERMINALS_ANCHORS_END_HPP_

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct end
    {
            static constexpr auto expression = static_string {"$"};
            using admitted_first_chars       = admitted_set<char>;

            template<typename... injected_children>
            using optimize = end;

            template<typename... injected_children>
            static constexpr auto match(auto& result) -> auto&
            {
                if (result.actual_iterator_end > result.query.end())
                {
                    result = false;
                    return result;
                }

                result
                    = (result.actual_iterator_end == result.query.end())
                      || *result.actual_iterator_end == '\n';

                return result;
            }
    };
} // namespace e_regex::terminals::anchors

#endif /* E_REGEX_TERMINALS_ANCHORS_END_HPP_*/
