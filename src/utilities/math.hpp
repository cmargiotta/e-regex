#ifndef E_REGEX_UTILITIES_MATH_HPP_
#define E_REGEX_UTILITIES_MATH_HPP_

#include <algorithm>

namespace e_regex
{
    consteval auto sum()
    {
        return 0;
    }

    consteval auto sum(auto... n)
    {
        return (n + ...);
    }

    consteval auto min()
    {
        return 0;
    }

    consteval auto min(auto n)
    {
        return n;
    }

    consteval auto min(auto n1, auto n2, auto... tail)
    {
        return min(std::min(n1, n2), tail...);
    }

    consteval auto max()
    {
        return 0;
    }

    consteval auto max(auto n)
    {
        return n;
    }

    consteval auto max(auto n1, auto n2, auto... tail)
    {
        return max(std::max(n1, n2), tail...);
    }

} // namespace e_regex

#endif /* E_REGEX_UTILITIES_MATH_HPP_*/
