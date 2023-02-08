#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <string>

namespace e_regex
{
    template<typename match_result, typename separator_matcher>
    class tokenization_result
    {
        public:
            class iterator
            {
                private:
                    match_result       res;
                    std::string_view   current;
                    separator_matcher& separator;

                public:
                    constexpr explicit iterator(match_result res, separator_matcher& separator)
                        : res {std::move(res)}, current {res.to_view()}, separator {separator}
                    {
                    }

                    constexpr iterator(const iterator& other) noexcept = default;
                    // constexpr auto operator=(const iterator& other) -> auto& = default;

                    constexpr iterator(iterator&& other) noexcept = default;
                    // constexpr auto operator=(iterator&& other) noexcept -> auto& = default;

                    constexpr auto operator++() noexcept -> iterator&
                    {
                        auto last = res.to_view();

                        res.next();
                        current = res.to_view();

                        if (current.begin() != last.end())
                        {
                            // Non-consecutive tokens, syntax error
                            current = {};
                        }

                        return *this;
                    }

                    constexpr auto operator==(const iterator& other) const noexcept -> bool
                    {
                        return current.begin() == other.current.begin();
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

                    constexpr void invalidate() noexcept
                    {
                        current = {};
                    }
            };

        private:
            match_result      matcher;
            separator_matcher separator;

        public:
            constexpr explicit tokenization_result(match_result matcher, separator_matcher separator)
                : matcher {matcher}, separator {std::move(separator)} {};

            constexpr auto begin() noexcept -> iterator
            {
                return iterator {matcher, separator};
            }

            constexpr auto end() noexcept -> iterator
            {
                iterator res {matcher, separator};
                res.invalidate();

                return res;
            }
    };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */