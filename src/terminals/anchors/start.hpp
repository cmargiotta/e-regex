#ifndef ANCHORS_START
#define ANCHORS_START

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct start : public terminal_common<start>
    {
            static constexpr auto match_(auto result)
            {
                result = (result.actual_iterator_end == result.query.begin());

                return result;
            }
    };
}// namespace e_regex::terminals::anchors

#endif /* ANCHORS_START */
