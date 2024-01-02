#ifndef UTILITIES_MAX
#define UTILITIES_MAX

namespace e_regex
{
    consteval auto max(auto n)
    {
        return n;
    }

    consteval auto max(auto n, auto m)
    {
        return n > m ? n : m;
    }

    consteval auto max(auto n, auto m, auto... tail)
    {
        return max(max(n, m), tail...);
    }
}// namespace e_regex

#endif /* UTILITIES_MAX */
