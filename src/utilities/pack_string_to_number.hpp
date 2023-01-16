#ifndef UTILITIES_PACK_STRING_TO_NUMBER
#define UTILITIES_PACK_STRING_TO_NUMBER

#include "static_string.hpp"

namespace e_regex
{
    template<typename string, std::size_t index = string::size - 1>
    struct pack_string_to_number;

    template<char head, char... tail, std::size_t index>
    struct pack_string_to_number<pack_string<head, tail...>, index>
    {
            static constexpr auto ten_power(std::size_t exp) -> std::size_t
            {
                std::size_t res = 1;

                while (exp-- != 0)
                {
                    res *= 10;
                }

                return res;
            }

            static constexpr std::size_t value
                = ((head - '0') * ten_power(index))
                  + pack_string_to_number<pack_string<tail...>, index - 1>::value;
    };

    template<std::size_t index>
    struct pack_string_to_number<pack_string<>, index>
    {
            static constexpr std::size_t value = 0;
    };
}// namespace e_regex

#endif /* UTILITIES_PACK_STRING_TO_NUMBER */
