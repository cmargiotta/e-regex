#ifndef E_REGEX_NODES_META_HPP_
#define E_REGEX_NODES_META_HPP_

#include <array>

namespace e_regex
{
    enum class policy
    {
        GREEDY,
        LAZY,
        POSSESSIVE,
        EXACT,
        NONE
    };

    template<typename _admission_set>
    struct meta
    {
            policy   policy_;
            unsigned minimum_match_size;
            unsigned maximum_match_size;

            using admission_set = _admission_set;
    };
} // namespace e_regex

#endif /* E_REGEX_NODES_META_HPP_*/
