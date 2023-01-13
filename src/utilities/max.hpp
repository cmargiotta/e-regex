#ifndef UTILITIES_MAX
#define UTILITIES_MAX

namespace e_regex
{
    consteval auto max()
    {
        return 0;
    }

    consteval auto max(auto n)
    {
        return n;
    }

    consteval auto max(auto n1, auto n2, auto... n3)
    {
        return max(std::max(n1, n2), n3...);
    }
}// namespace e_regex

#endif /* UTILITIES_MAX */
