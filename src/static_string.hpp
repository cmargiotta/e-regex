#ifndef STATIC_STRING
#define STATIC_STRING

#include <array>

namespace e_regex
{
    template<char... data>
    struct static_string
    {
            static constexpr std::array<char, sizeof...(data)> string = {data...};
    };

    template<typename S1, typename S2>
    struct merge_static_strings;

    template<char... data1, char... data2>
    struct merge_static_strings<static_string<data1...>, static_string<data2...>>
    {
            using type = static_string<data1..., data2...>;
    };

    template<typename S1, typename S2>
    using merge_static_strings_t = typename merge_static_strings<S1, S2>::type;

}// namespace e_regex

#endif /* STATIC_STRING */
