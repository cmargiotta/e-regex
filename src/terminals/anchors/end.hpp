#ifndef TERMINALS_ANCHORS_END
#define TERMINALS_ANCHORS_END

#include "terminals/common.hpp"

namespace e_regex::terminals::anchors
{
    struct end
    {
            using expression           = pack_string<'$'>;
            using admitted_first_chars = admitted_set<char>;

            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end == result.query.end());

                return result;
            }
    };
}// namespace e_regex::terminals::anchors

#endif /* TERMINALS_ANCHORS_END */
