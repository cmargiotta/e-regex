#ifndef STATIC_STRING
#define STATIC_STRING

#include <array>
#include <utility>

#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<char... data>
    struct pack_string
    {
            static constexpr auto       size   = sizeof...(data);
            static constexpr std::array string = {data...};
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

    template<std::size_t size_>
    struct static_string
    {
            static constexpr auto  size = size_ - 1;
            std::array<char, size> data;

            constexpr static_string(const char (&data)[size_]) noexcept
            {
                std::copy(data, data + size, this->data.begin());
            }
    };

    template<literal_string_view string>
    constexpr auto to_static_string() noexcept
    {
        static_string<string.size> result;
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
