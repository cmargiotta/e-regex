#ifndef ANCHORS_START
#define ANCHORS_START

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct start
    {
            using expression           = pack_string<'^'>;
            using admitted_first_chars = admitted_set<char>;

            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end == result.query.begin());

                return result;
            }
    };
}// namespace e_regex::terminals::anchors

#endif /* ANCHORS_START */
