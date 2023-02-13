#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <array>
#include <string>
#include <vector>

#include "static_string.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<typename match_result, typename separator_matcher, typename Char_Type = char>
    class tokenization_result
    {
        public:
            class iterator
            {
                private:
                    match_result                   res;
                    literal_string_view<Char_Type> current;
                    separator_matcher              separator_verifier;

                public:
                    constexpr explicit iterator(match_result res, separator_matcher separator_verifier)
                        : res {std::move(res)},
                          current {res.to_view()},
                          separator_verifier {std::move(separator_verifier)}
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
                        current = res.to_view();

                        if (current.begin() != last.end())
                        {
                            // Non-consecutive tokens, possible syntax error
                            literal_string_view separator {last.end(), current.begin()};

                            if (!separator_verifier(separator).is_accepted())
                            {
                                // Separator is not accepted from the given separator matcher,
                                // syntax error found!
                                current = {};
                            }
                        }

                        return *this;
                    }

                    constexpr auto operator==(const iterator& other) const noexcept -> bool
                    {
                        return current.begin() == other.current.begin()
                               || (current.empty() && other.current.empty());
                    }

                    constexpr auto operator!=(const iterator& other) const noexcept -> bool
                    {
                        return !operator==(other);
                    }

                    constexpr auto operator*() noexcept
                    {
                        return current;
                    }

                    constexpr auto operator*() const noexcept
                    {
                        return current;
                    }

                    constexpr operator bool() const noexcept
                    {
                        return !current.empty();
                    }

                    constexpr void invalidate() noexcept
                    {
                        current = {};
                    }
            };

        private:
            match_result      matcher;
            separator_matcher separator_verifier;

        public:
            constexpr explicit tokenization_result(match_result      matcher,
                                                   separator_matcher separator_verifier)
                : matcher {matcher}, separator_verifier {std::move(separator_verifier)} {};

            constexpr auto begin() const noexcept -> iterator
            {
                return iterator {matcher, separator_verifier};
            }

            constexpr auto end() const noexcept -> iterator
            {
                iterator res {matcher, separator_verifier};
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

    template<literal_string_view... data>
    struct token_container
    {
            static constexpr std::array tokens {data...};
    };

    template<typename tokens1, typename tokens2>
    struct merge_tokens;

    template<literal_string_view... data1, literal_string_view... data2>
    struct merge_tokens<token_container<data1...>, token_container<data2...>>
    {
            using type = token_container<data1..., data2...>;
    };

    template<auto matcher, auto separator_matcher, literal_string_view string>
    struct prebuilt_tokenization_result
    {
            static constexpr auto res   = tokenization_result {matcher(string), separator_matcher};
            static constexpr auto token = *(res.begin());

            using tokens =
                typename merge_tokens<token_container<token>,
                                      typename prebuilt_tokenization_result<
                                          matcher,
                                          separator_matcher,
                                          literal_string_view<> {token.end(), string.end()}>::tokens>::type;
    };

    template<auto matcher, auto separator_matcher, literal_string_view string>
    requires(string.empty()) struct prebuilt_tokenization_result<matcher, separator_matcher, string>
    {
            using tokens = token_container<>;
    };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */
