#ifndef E_REGEX_TOKENIZATION_TOKENIZER_HPP_
#define E_REGEX_TOKENIZATION_TOKENIZER_HPP_

#include <tuple>

#include "iterator.hpp"
#include "token.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex::tokenization
{
    template<token_definition auto... _tokens>
    class tokenizer
    {
        public:
            static constexpr auto match(literal_string_view<> input)
            {
                return tokenization::iterator<_tokens...> {input};
            }

            constexpr auto operator()(literal_string_view<> input) const
            {
                return match(input);
            }
    };
} // namespace e_regex::tokenization
#endif /* E_REGEX_TOKENIZATION_TOKENIZER_HPP_*/
