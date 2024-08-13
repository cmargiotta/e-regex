#ifndef E_REGEX_UTILITIES_STATIC_STRING_HPP_
#define E_REGEX_UTILITIES_STATIC_STRING_HPP_

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

#include "literal_string_view.hpp"

namespace e_regex
{
    template<std::size_t size_, typename Char_Type = char>
    struct static_string
    {
            static constexpr auto size = size_;
            std::conditional_t<(size > 0), std::array<char, size>, std::tuple<>> data;

            template<auto S>
            constexpr static_string(const Char_Type (&data)[S]) noexcept
            {
                if constexpr (size > 0)
                {
                    std::copy(data, data + size, this->data.begin());
                }
            }

            constexpr static_string(Char_Type data) noexcept
            {
                this->data[0] = data;
            }

            template<auto S = size>
                requires(size != 0)
            constexpr static_string(const std::array<Char_Type, size>& data) noexcept
                : data {data}
            {}

            // template<literal_string_view string>
            // constexpr static_string();

            constexpr static_string() noexcept = default;

            constexpr operator literal_string_view<Char_Type>() const noexcept
            {
                return to_view();
            }

            constexpr auto to_view() const noexcept
            {
                if constexpr (size > 0)
                {
                    return literal_string_view<Char_Type> {
                        data.begin(), data.end()};
                }
                else
                {
                    return literal_string_view<Char_Type> {};
                }
            }

            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return size == 0;
            }

            template<std::size_t begin, std::size_t end = size>
            constexpr auto substring() const noexcept
            {
                if constexpr (size > 0)
                {
                    static_string<end - begin> result;

                    std::copy(data.begin() + begin,
                              data.begin() + end,
                              result.data.begin());
                    return result;
                }
                else
                {
                    return *this;
                }
            }
    };

    template<auto S1, auto S2, typename C>
    constexpr auto operator+(static_string<S1, C> first,
                             static_string<S2, C> other)
    {
        if constexpr (first.size == 0)
        {
            return other;
        }
        else if constexpr (other.size == 0)
        {
            return first;
        }
        else
        {
            static_string<S1 + S2> result {};

            std::copy(first.data.begin(),
                      first.data.end(),
                      result.data.begin());
            std::copy(other.data.begin(),
                      other.data.end(),
                      result.data.begin() + S1);

            return result;
        }
    }

    template<auto S1, auto S2, typename C>
    constexpr auto operator+(const C (&first)[S1],
                             static_string<S2, C> other)
    {
        return static_string {first} + other;
    }

    template<auto S1, auto S2, typename C>
    constexpr auto operator+(static_string<S1, C> first,
                             const C (&other)[S2])
    {
        return first + static_string {other};
    }

    template<auto S2, typename C>
    constexpr auto operator+(C first, static_string<S2, C> other)
    {
        return static_string {first} + other;
    }

    template<auto S1, typename C>
    constexpr auto operator+(static_string<S1, C> first, C other)
    {
        return first + static_string {other};
    }

    // CTAD
    template<typename C>
    static_string(C data) -> static_string<1, C>;

    template<auto S, typename C>
    static_string(const C (&data)[S]) -> static_string<S - 1, C>;

} // namespace e_regex

#endif /* E_REGEX_UTILITIES_STATIC_STRING_HPP_*/
