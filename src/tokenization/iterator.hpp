#ifndef E_REGEX_TOKENIZATION_ITERATOR_HPP_
#define E_REGEX_TOKENIZATION_ITERATOR_HPP_

#include <algorithm>
#include <array>
#include <concepts>
#include <iostream>
#include <tuple>
#include <type_traits>

#include "e_regex.hpp"
#include "token.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex::tokenization
{
    namespace __private
    {
        template<typename Regex, typename Token_Type>
        struct token_matcher
        {
                Token_Type                  type;
                [[no_unique_address]] Regex matcher;

                constexpr token_matcher(Token_Type type, Regex /*regex*/)
                    : type {type}
                {}
        };

        template<token_definition auto... tokens>
        struct separator_splitter;

        template<is_token_c auto token, token_definition auto... tail>
        struct separator_splitter<token, tail...>
        {
                using rec = separator_splitter<tail...>;

                static constexpr auto tokens = std::tuple_cat(
                    std::make_tuple(token_matcher {
                        token.type,
                        // Adding ^ to alway match the beginning of
                        // current
                        e_regex::regex<static_string {"^"} + token.matcher> {}}),
                    rec::tokens);
                static constexpr auto separators = rec::separators;
        };

        template<is_separator_c auto token, token_definition auto... tail>
        struct separator_splitter<token, tail...>
        {
                using rec = separator_splitter<tail...>;

                static constexpr auto tokens     = rec::tokens;
                static constexpr auto separators = std::tuple_cat(
                    std::make_tuple(
                        e_regex::regex<static_string {"^"} + token.matcher> {}),
                    rec::separators);
        };

        template<>
        struct separator_splitter<>
        {
                static constexpr auto tokens     = std::tuple<> {};
                static constexpr auto separators = std::tuple<> {};
        };
    } // namespace __private

    template<token_definition auto... _tokens>
        requires(sizeof...(_tokens) > 0)
    class iterator
    {
        private:
            using data = __private::separator_splitter<_tokens...>;

        public:
            using token_type_t
                = decltype(std::get<0>(data::tokens).type);

            static_assert(
                std::apply(
                    [](auto... args) {
                        return (
                            ...
                            && std::same_as<token_type_t, decltype(args.type)>);
                    },
                    data::tokens),
                "Tokens must have the same class type");

            struct token_t
            {
                    literal_string_view<> value;
                    token_type_t          type;
            };

        private:
            literal_string_view<> current;
            literal_string_view<> original_input;
            token_t               last_match;
            bool                  valid = true;

            [[nodiscard]] constexpr auto match_separator() -> bool
            {
                auto matches = std::apply(
                    [this](auto... regex) {
                        return std::array {regex(current).to_view()...};
                    },
                    data::separators);

                auto best_match = std::max_element(
                    matches.begin(),
                    matches.end(),
                    [](literal_string_view<> first,
                       literal_string_view<> second) {
                        return first.size() < second.size();
                    });

                if (best_match->size() > 0)
                {
                    current = literal_string_view<> {
                        best_match->end(), current.end()};
                    return true;
                }

                return false;
            }

            [[nodiscard]] constexpr auto match_token() -> bool
            {
                auto matches = std::apply(
                    [this](auto... token) {
                        return std::array {token_t {
                            .value = token.matcher(current).to_view(),
                            .type  = token.type}...};
                    },
                    data::tokens);

                auto best_match = std::max_element(
                    matches.begin(),
                    matches.end(),
                    [](token_t& first, token_t& second) {
                        return first.value.size() < second.value.size();
                    });

                if (best_match->value.size() > 0)
                {
                    current = literal_string_view<> {
                        best_match->value.end(), current.end()};
                    last_match = *best_match;
                    return true;
                }

                return false;
            }

        public:
            constexpr explicit iterator(literal_string_view<> input)
                : current {input}, original_input {input}
            {
                if (!input.empty())
                {
                    operator++();
                }
            }

            constexpr iterator(const iterator& other) noexcept = default;
            constexpr auto operator=(const iterator& other)
                -> iterator&                              = default;
            constexpr iterator(iterator&& other) noexcept = default;
            constexpr auto operator=(iterator&& other) noexcept
                -> iterator&               = default;
            constexpr ~iterator() noexcept = default;

            constexpr auto begin() const -> iterator
            {
                return iterator {original_input};
            }

            constexpr auto operator++() noexcept -> iterator&
            {
                if (!match_separator()
                    && original_input.begin_ != current.begin())
                {
                    // Invalidate only if a separator is missing
                    // after  a token, aka: this is not the first
                    // match
                    invalidate();
                }
                else if (!match_token())
                {
                    invalidate();
                }

                return *this;
            }

            constexpr auto end() const -> iterator
            {
                return iterator {{}}; // Invalid by definition
            }

            constexpr auto operator==(const iterator& other) const noexcept
                -> bool
            {
                return (current == other.current
                        && last_match.value == other.last_match.value);
            }

            constexpr auto operator!=(const iterator& other) const noexcept
                -> bool
            {
                return !operator==(other);
            }

            constexpr auto operator->() const noexcept
            {
                return &last_match;
            }

            constexpr auto operator*() const noexcept
            {
                return last_match;
            }

            constexpr operator bool() const noexcept
            {
                return !last_match.value.empty();
            }

            constexpr void invalidate() noexcept
            {
                current    = {};
                last_match = {};
            }
    };
} // namespace e_regex::tokenization

#endif /* E_REGEX_TOKENIZATION_ITERATOR_HPP_*/
