#ifndef E_REGEX_UTILITIES_PACK_STRING_TO_NUMBER_HPP_
#define E_REGEX_UTILITIES_PACK_STRING_TO_NUMBER_HPP_

#include "static_string.hpp"

namespace e_regex
{
    template<typename string, unsigned index = string::size - 1>
    struct pack_string_to_number;

    template<char head, char... tail, unsigned index>
    struct pack_string_to_number<pack_string<head, tail...>, index>
    {
            static consteval auto ten_power(unsigned exp) -> unsigned
            {
                unsigned res = 1;

                while (exp-- != 0)
                {
                    res *= 10;
                }

                return res;
            }

            static inline const constinit unsigned value
                = ((head - '0') * ten_power(index))
                  + pack_string_to_number<pack_string<tail...>, index - 1>::value;
    };

    template<unsigned index>
    struct pack_string_to_number<pack_string<>, index>
    {
            static constexpr unsigned value = 0;
    };
} // namespace e_regex

#endif /* E_REGEX_UTILITIES_PACK_STRING_TO_NUMBER_HPP_*/
