#ifndef E_REGEX_NODES_TERMINALS_ANCHORS_END_HPP_
#define E_REGEX_NODES_TERMINALS_ANCHORS_END_HPP_

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct end
    {
            static constexpr auto expression = static_string {"$"};
            using admitted_first_chars       = admitted_set<char>;

            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end
                          == result.query.end());

                return result;
            }
    };
} // namespace e_regex::terminals::anchors

#endif /* E_REGEX_NODES_TERMINALS_ANCHORS_END_HPP_*/
