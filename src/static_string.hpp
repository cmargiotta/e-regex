#ifndef STATIC_STRING
#define STATIC_STRING

#include <array>
#include <string_view>
#include <utility>

#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<std::size_t size_, typename Char_Type = char>
    struct static_string
    {
            static constexpr auto  size = size_ - 1;
            std::array<char, size> data;

            constexpr static_string(const Char_Type (&data)[size_]) noexcept
            {
                std::copy(data, data + size, this->data.begin());
            }

            constexpr static_string(const std::array<Char_Type, size>& data) noexcept: data {data}
            {
            }

            constexpr static_string() noexcept = default;

            constexpr operator literal_string_view<Char_Type>() const noexcept
            {
                return to_view();
            }

            constexpr auto to_view() const noexcept
            {
                return literal_string_view<Char_Type> {data.begin(), data.end()};
            }

            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return size == 0;
            }

            template<std::size_t begin, std::size_t end = size>
            constexpr auto substring() const noexcept
            {
                static_string<end - begin + 1> result;

                std::copy(data.begin() + begin, data.begin() + end, result.data.begin());
                return result;
            }
    };

    template<char... data>
    struct pack_string
    {
            static constexpr auto size   = sizeof...(data);
            static constexpr auto string = static_string<size + 1> {std::array {data...}};
    };

    template<typename S1, typename S2>
    struct merge_pack_strings;

    template<char... data1, char... data2>
    struct merge_pack_strings<pack_string<data1...>, pack_string<data2...>>
    {
            using type = pack_string<data1..., data2...>;
    };

    template<typename S1, typename S2>
    using merge_pack_strings_t = typename merge_pack_strings<S1, S2>::type;

    template<typename separator, typename... strings>
    struct concatenate_pack_strings;

    template<typename separator, typename string>
    struct concatenate_pack_strings<separator, string>
    {
            using type = string;
    };

    template<typename separator>
    struct concatenate_pack_strings<separator>
    {
            using type = pack_string<>;
    };

    template<typename separator, typename string, typename... strings>
    struct concatenate_pack_strings<separator, string, strings...>
    {
            using first_part = merge_pack_strings_t<string, separator>;
            using type
                = merge_pack_strings_t<first_part,
                                       typename concatenate_pack_strings<separator, strings...>::type>;
    };

    template<typename separator, typename... strings>
    using concatenate_pack_strings_t = typename concatenate_pack_strings<separator, strings...>::type;

    template<literal_string_view string>
    constexpr auto to_static_string() noexcept
    {
        static_string<string.size() + 1> result;
        std::copy(string.begin(), string.end(), result.data.begin());

        return result;
    }

    template<std::size_t size>
    constexpr auto to_static_string(auto string) noexcept
    {
        static_string<size + 1> result;

        std::copy(string.begin(), string.end(), result.data.begin());

        return result;
    }

    template<auto instance, typename indices = std::make_index_sequence<instance.size>>
    struct build_pack_string;

    template<auto instance, std::size_t... indices>
    struct build_pack_string<instance, std::index_sequence<indices...>>
    {
            using type = pack_string<instance.data[indices]...>;
    };

    template<auto instance>
    using build_pack_string_t = typename build_pack_string<instance>::type;

}// namespace e_regex

#endif /* STATIC_STRING */
