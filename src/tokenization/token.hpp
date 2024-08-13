#ifndef E_REGEX_TOKENIZATION_TOKEN_HPP_
#define E_REGEX_TOKENIZATION_TOKEN_HPP_

#include <array>

#include "utilities/literal_string_view.hpp"
#include "utilities/static_string.hpp"

namespace e_regex
{
    template<typename T, auto size>
    struct token
    {
            T                   type;
            static_string<size> matcher;

            template<auto S>
            constexpr token(T type, const char (&matcher)[S])
                : type {type}, matcher {matcher}
            {}
    };

    template<typename T, auto S>
    token(T type, const char (&matcher)[S]) -> token<T, S - 1>;

    template<auto size>
    struct separator
    {
            static_string<size> matcher;

            template<auto S>
            constexpr separator(const char (&matcher)[S])
                : matcher {matcher}
            {}
    };

    template<auto S>
    separator(const char (&matcher)[S]) -> separator<S - 1>;

    template<typename T>
    struct is_token : public std::false_type
    {};

    template<typename T, auto size>
    struct is_token<e_regex::token<T, size>> : public std::true_type
    {};

    template<typename T>
    concept is_token_c = is_token<T>::value;

    template<typename T>
    struct is_separator : public std::false_type
    {};

    template<auto size>
    struct is_separator<e_regex::separator<size>> : public std::true_type
    {};

    template<typename T>
    concept is_separator_c = is_separator<T>::value;

    template<typename T>
    concept token_definition = is_separator_c<T> || is_token_c<T>;

} // namespace e_regex

#endif /* E_REGEX_TOKENIZATION_TOKEN_HPP_*/
