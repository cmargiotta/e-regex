#ifndef TOKENIZATION_PREBUILT_RESULT_HPP
#define TOKENIZATION_PREBUILT_RESULT_HPP

#include "result.hpp"
#include "static_string.hpp"
#include "token.hpp"

namespace e_regex::tokenization
{
    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
    struct prebuilt_result
    {
            static consteval auto get_string(auto token) noexcept
            {
                if constexpr (has_end<decltype(token)>)
                {
                    return token;
                }
                else
                {
                    return token.string;
                }
            }

            static constexpr auto query = string.to_view();
            static constexpr auto res   = result<matcher, separator_matcher, Token_Type> {query};

            static constexpr auto token_data   = *(res.begin());
            static constexpr auto token_string = get_string(token_data);
            static constexpr auto token_end    = token_string.end() - string.data.begin();
            static constexpr auto token_static_string
                = to_static_string<token_string.size()>(token_string);

            static constexpr auto token {build_token<Token_Type, token_static_string>(token_data)};

            using tokens =
                typename merge_tokens<token_container<Token_Type, token>,
                                      typename prebuilt_result<matcher,
                                                               separator_matcher,
                                                               string.template substring<token_end>(),
                                                               Token_Type>::tokens>::type;
    };

    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
        requires(string.empty())
    struct prebuilt_result<matcher, separator_matcher, string, Token_Type>
    {
            using tokens = token_container<Token_Type>;
    };
}// namespace e_regex::tokenization

#endif /* TOKENIZATION_PREBUILT_RESULT_HPP */
