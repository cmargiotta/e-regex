#ifndef TOKENIZATION_TOKEN_HPP
#define TOKENIZATION_TOKEN_HPP

#include <array>

#include "static_string.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex::tokenization
{
    template<typename Token_Type, typename String_Type = literal_string_view<char>>
    struct token
    {
            Token_Type  type;
            String_Type string;
    };

    template<typename String_Type>
    struct token<void, String_Type>
    {
            String_Type string;
    };

    template<typename Token_Type, auto... data>
    struct token_container
    {
            static constexpr std::array tokens {token<Token_Type> {data.type, data.string.to_view()}...};
    };

    template<auto... data>
    struct token_container<void, data...>
    {
            static constexpr std::array tokens {data.to_view()...};
    };

    template<typename tokens1, typename tokens2>
    struct merge_tokens;

    template<typename Token_Type, auto... data1, auto... data2>
    struct merge_tokens<token_container<Token_Type, data1...>, token_container<Token_Type, data2...>>
    {
            using type = token_container<Token_Type, data1..., data2...>;
    };

    template<typename T>
    concept has_end = requires(T d) { d.end(); };

    template<typename Token_Type, static_string string>
    consteval auto build_token(auto token_with_literal)
        requires(!std::is_same_v<Token_Type, void>)
    {
        return token<Token_Type, decltype(string)> {.type = token_with_literal.type, .string = string};
    };

    template<std::same_as<void> Token_Type, static_string string>
    consteval auto build_token(auto /*unused*/)
    {
        return string;
    };
}// namespace e_regex::tokenization

#endif /* TOKENIZATION_TOKEN_HPP */
