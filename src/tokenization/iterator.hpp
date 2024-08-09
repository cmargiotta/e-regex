#ifndef TOKENIZATION_RUNTIME_ITERATOR_HPP
#define TOKENIZATION_RUNTIME_ITERATOR_HPP

#include <array>
#include <type_traits>

#include "utilities/literal_string_view.hpp"

namespace e_regex::tokenization
{
    template<typename match_result, typename token_type, auto separator_verifier, std::array type_map>
    class iterator
    {
        private:
            using token_class = decltype(type_map)::value_type;

            match_result res;
            token_type   current;

            [[nodiscard]] static constexpr auto build_token(const match_result& res) noexcept
            {
                std::size_t index = 1;

                for (; index <= match_result::groups(); ++index)
                {
                    if (!res[index].empty())
                    {
                        break;
                    }
                }

                return decltype(current) {.type = type_map[index - 1], .string = res.to_view()};
            }

        public:
            constexpr explicit iterator(match_result res): res {res}, current {build_token(res)}
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
                return current;
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
}// namespace e_regex::tokenization

#endif /* TOKENIZATION_RUNTIME_ITERATOR_HPP */
