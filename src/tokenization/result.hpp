#ifndef TOKENIZATION_RESULT_HPP
#define TOKENIZATION_RESULT_HPP

#include "iterator.hpp"
#include "token.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex::tokenization
{
    // If Token_Type is an enum, it MUST contain at least the number of groups of regex of
    // consecutive values starting from 0
    template<auto matcher, auto separator_verifier, typename Token_Type_, typename Char_Type = char>
    class result
    {
        private:
            using match_result = decltype(matcher(std::declval<literal_string_view<>>()));

        public:
            using token_type = token<Token_Type_, literal_string_view<Char_Type>>;
            using iter       = iterator<match_result, token_type, Token_Type_, separator_verifier>;

        private:
            match_result regex_result;

        public:
            constexpr explicit result(literal_string_view<> expression)
                : regex_result {matcher(expression)} {};

            constexpr auto begin() const noexcept -> iter
            {
                return iter {regex_result};
            }

            constexpr auto end() const noexcept -> iter
            {
                iter res {regex_result};
                res.invalidate();

                return res;
            }

            constexpr auto count() const noexcept
            {
                std::size_t result = 0;
                auto        i      = begin();

                while (i)
                {
                    ++result;
                    ++i;
                }

                return result;
            }
    };
}// namespace e_regex::tokenization

#endif /* TOKENIZATION_RESULT_HPP */
