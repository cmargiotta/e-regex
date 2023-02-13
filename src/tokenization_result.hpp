#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <array>
#include <string>
#include <vector>

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

        public:
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

    // template<tokenization_result result>
    // class prebuilt_tokenization_result
    // {
    //     private:
    //         std::array<literal_string_view, result.count()> tokens;

    //     public:
    //         constexpr prebuilt_tokenization_result()
    //         {
    //             auto tokens_iterator = tokens.begin();

    //             for (auto i: result)
    //             {
    //                 *tokens_iterator = *i;
    //                 ++tokens_iterator;
    //             }
    //         }

    //         constexpr auto begin() const noexcept
    //         {
    //             return tokens.begin();
    //         }

    //         constexpr auto end() const noexcept
    //         {
    //             return tokens.end();
    //         }
    // };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */
