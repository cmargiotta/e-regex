#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <array>
#include <string>
#include <vector>

#include "static_string.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex
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

    // If Token_Type is an enum, it MUST contain at least the number of groups of regex of
    // consecutive values starting from 0
    template<auto matcher, auto separator_verifier, typename Token_Type_, typename Char_Type = char>
    class tokenization_result
    {
        private:
            using match_result = decltype(matcher(std::declval<literal_string_view<>>()));

        public:
            using token_type = token<Token_Type_, literal_string_view<Char_Type>>;

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
    concept has_end = requires(T d)
    {
        d.end();
    };

    template<typename Token_Type, static_string string>
    consteval auto build_token(auto token_with_literal) requires(!std::is_same_v<Token_Type, void>)
    {
        return e_regex::token<Token_Type, decltype(string)> {token_with_literal.type, string};
    };

    template<std::same_as<void> Token_Type, static_string string>
    consteval auto build_token(auto /*unused*/)
    {
        return string;
    };

    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
    struct prebuilt_tokenization_result
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
            static constexpr auto res
                = tokenization_result<matcher, separator_matcher, Token_Type> {query};

            static constexpr auto token_data   = *(res.begin());
            static constexpr auto token_string = get_string(token_data);
            static constexpr auto token_end    = token_string.end() - string.data.begin();
            static constexpr auto token_static_string
                = to_static_string<token_string.size()>(token_string);

            static constexpr auto token {build_token<Token_Type, token_static_string>(token_data)};

            using tokens = typename merge_tokens<
                token_container<Token_Type, token>,
                typename prebuilt_tokenization_result<matcher,
                                                      separator_matcher,
                                                      string.template substring<token_end>(),
                                                      Token_Type>::tokens>::type;
    };

    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
    requires(string.empty()) struct prebuilt_tokenization_result<matcher, separator_matcher, string, Token_Type>
    {
            using tokens = token_container<Token_Type>;
    };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */
