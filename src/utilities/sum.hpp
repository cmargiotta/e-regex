#ifndef UTILITIES_SUM
#define UTILITIES_SUM

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
}// namespace e_regex

#endif /* UTILITIES_SUM */
