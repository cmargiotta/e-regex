#ifndef E_REGEX_UTILITIES_LITERAL_STRING_VIEW_HPP_
#define E_REGEX_UTILITIES_LITERAL_STRING_VIEW_HPP_

#include <functional>
#include <string_view>

#include "macros.hpp"

namespace e_regex
{
    template<typename Type = char>
    struct literal_string_view
    {
            using iterator = const Type*;

            iterator begin_;
            iterator end_;

            constexpr EREGEX_ALWAYS_INLINE literal_string_view()
                : begin_ {nullptr}, end_ {nullptr}
            {}

            constexpr EREGEX_ALWAYS_INLINE
                literal_string_view(iterator begin_, iterator end_)
                : begin_ {begin_}, end_ {end_}
            {}

            template<unsigned size>
            constexpr EREGEX_ALWAYS_INLINE
                literal_string_view(const char (&data)[size]) noexcept
                : begin_ {std::begin(data)}, end_ {std::end(data) - 1}
            {}

            constexpr EREGEX_ALWAYS_INLINE
                literal_string_view(std::string_view data)
                : begin_ {&*data.begin()}, end_ {&*data.end()}
            {
                static_assert(
                    std::same_as<Type, std::decay_t<decltype(*data.begin())>>);
            }

            constexpr literal_string_view(const literal_string_view& other) noexcept
                = default;
            constexpr literal_string_view(literal_string_view&& other) noexcept
                = default;
            constexpr auto operator=(const literal_string_view& other) noexcept
                -> literal_string_view& = default;
            constexpr auto operator=(literal_string_view&& other) noexcept
                -> literal_string_view&               = default;
            constexpr ~literal_string_view() noexcept = default;

            constexpr auto EREGEX_ALWAYS_INLINE begin() noexcept
            {
                return begin_;
            }

            constexpr auto EREGEX_ALWAYS_INLINE end() noexcept
            {
                return end_;
            }

            constexpr auto EREGEX_ALWAYS_INLINE begin() const noexcept
            {
                return begin_;
            }

            constexpr auto EREGEX_ALWAYS_INLINE end() const noexcept
            {
                return end_;
            }

            constexpr auto EREGEX_ALWAYS_INLINE size() const noexcept
            {
                return end_ - begin_;
            }

            constexpr EREGEX_ALWAYS_INLINE operator std::string_view() const noexcept
            {
                return std::string_view {begin(), end()};
            }

            constexpr EREGEX_ALWAYS_INLINE bool
                operator==(literal_string_view other) const
            {
                if (begin() == other.begin() && end() == other.end())
                {
                    return true;
                }

                if (size() != other.size())
                {
                    return false;
                }

                for (auto i = 0; i < size(); ++i)
                {
                    if (begin()[i] != other.begin()[i])
                    {
                        return false;
                    }
                }

                return true;
            }

            constexpr EREGEX_ALWAYS_INLINE auto empty() const noexcept
            {
                return size() == 0;
            }
    };
} // namespace e_regex


#endif /* E_REGEX_UTILITIES_LITERAL_STRING_VIEW_HPP_*/
