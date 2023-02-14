#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <array>
#include <string>
#include <vector>

#include "static_string.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<typename Token_Type, typename Char_Type = char>
    struct token
    {
            Token_Type                     type;
            literal_string_view<Char_Type> string;
    };

    template<typename Char_Type>
    struct token<void, Char_Type>
    {
            literal_string_view<Char_Type> string;
    };

    // If Token_Type is an enum, it MUST contain at least the number of groups of regex of
    // consecutive values starting from 0
    template<auto matcher, auto separator_verifier, typename Token_Type_, typename Char_Type = char>
    class tokenization_result
    {
        private:
            using match_result = decltype(matcher(std::declval<literal_string_view<>>()));

        public:
            using token_type = token<Token_Type_, Char_Type>;

            class iterator
            {
                private:
                    match_result res;
                    token_type   current;

                    [[nodiscard]] static constexpr auto build_token(const match_result& res) noexcept
                    {
                        if constexpr (std::is_same_v<Token_Type_, void>)
                        {
                            // void token type, no need to match a group
                            return decltype(current) {res.to_view()};
                        }
                        else
                        {
                            std::size_t index = 0;

                            if constexpr (match_result::groups() != 0)
                            {
                                index = 1;
                                while (res[index].empty())
                                {
                                    ++index;
                                }
                                index--;
                            }

                            return decltype(current) {.type   = static_cast<Token_Type_>(index),
                                                      .string = res.to_view()};
                        }
                    }

                public:
                    constexpr explicit iterator(match_result res)
                        : res {std::move(res)}, current {build_token(res)}
                    {
                    }

                    constexpr iterator(const iterator& other) noexcept               = default;
                    constexpr auto operator=(const iterator& other) -> iterator&     = default;
                    constexpr iterator(iterator&& other) noexcept                    = default;
                    constexpr auto operator=(iterator&& other) noexcept -> iterator& = default;
                    constexpr ~iterator() noexcept                                   = default;

                    constexpr auto operator++() noexcept -> iterator&
                    {
                        auto last = res.to_view();

                        res.next();
                        current = build_token(res);

                        if (current.string.begin() != last.end())
                        {
                            // Non-consecutive tokens, possible syntax error
                            literal_string_view separator {last.end(), current.string.begin()};

                            if (!separator_verifier(separator).is_accepted())
                            {
                                // Separator is not accepted from the given separator matcher,
                                // syntax error found!
                                invalidate();
                            }
                        }

                        return *this;
                    }

                    constexpr auto operator==(const iterator& other) const noexcept -> bool
                    {
                        return current.string.begin() == other.current.string.begin()
                               || (current.string.empty() && other.current.string.empty());
                    }

                    constexpr auto operator!=(const iterator& other) const noexcept -> bool
                    {
                        return !operator==(other);
                    }

                    constexpr auto operator*() const noexcept
                    {
                        if constexpr (std::is_same_v<Token_Type_, void>)
                        {
                            return current.string;
                        }
                        else
                        {
                            return current;
                        }
                    }

                    constexpr operator bool() const noexcept
                    {
                        return !current.string.empty();
                    }

                    constexpr void invalidate() noexcept
                    {
                        current = {};
                    }
            };

        private:
            match_result regex_result;

        public:
            constexpr explicit tokenization_result(literal_string_view<> expression)
                : regex_result {matcher(expression)} {};

            constexpr auto begin() const noexcept -> iterator
            {
                return iterator {regex_result};
            }

            constexpr auto end() const noexcept -> iterator
            {
                iterator res {regex_result};
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

    template<typename Token_Type, Token_Type... data>
    struct token_container
    {
            static constexpr std::array tokens {data...};
    };

    template<typename Char_Type, token<void, Char_Type>... data>
    struct token_container<token<void, Char_Type>, data...>
    {
            static constexpr std::array tokens {data.string...};
    };

    template<typename tokens1, typename tokens2>
    struct merge_tokens;

    template<typename Token_Type, Token_Type... data1, Token_Type... data2>
    struct merge_tokens<token_container<Token_Type, data1...>, token_container<Token_Type, data2...>>
    {
            using type = token_container<Token_Type, data1..., data2...>;
    };

    template<auto d>
    concept has_end = requires
    {
        d.end();
    };

    template<auto matcher, auto separator_matcher, literal_string_view string, typename Token_Type>
    struct prebuilt_tokenization_result
    {
            static constexpr auto res
                = tokenization_result<matcher, separator_matcher, Token_Type> {string};
            using token_t = e_regex::token<Token_Type>;
            using token   = token_container<token_t, token_t {*(res.begin())}>;

            template<auto token>
            static consteval auto get_end() noexcept
            {
                if constexpr (has_end<token>)
                {
                    return token.end();
                }
                else
                {
                    return token.string.end();
                }
            }

            using tokens =
                typename merge_tokens<token,
                                      typename prebuilt_tokenization_result<
                                          matcher,
                                          separator_matcher,
                                          literal_string_view<> {get_end<token::tokens[0]>(), string.end()},
                                          Token_Type>::tokens>::type;
    };

    template<auto matcher, auto separator_matcher, literal_string_view string, typename Token_Type>
    requires(string.empty()) struct prebuilt_tokenization_result<matcher, separator_matcher, string, Token_Type>
    {
            using tokens = token_container<token<Token_Type>>;
    };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */
