#ifndef E_REGEX_UTILITIES_PACK_STRING_HPP_
#define E_REGEX_UTILITIES_PACK_STRING_HPP_

#include "static_string.hpp"

namespace e_regex
{
    template<char... data>
    struct pack_string;

    namespace __private
    {
        /* Merge two or more pack strings into one */
        template<typename... Strings>
        struct merge_pack_strings;

        template<char... data1, char... data2, typename... Strings>
        struct merge_pack_strings<pack_string<data1...>, pack_string<data2...>, Strings...>
        {
                using type =
                    typename merge_pack_strings<pack_string<data1..., data2...>,
                                                Strings...>::type;
        };

        template<typename String>
        struct merge_pack_strings<String>
        {
                using type = String;
        };

        template<>
        struct merge_pack_strings<>
        {
                using type = pack_string<>;
        };

        /* Merge two or more pack strings into one and add a separator
         * between them */
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

        template<typename separator, typename... strings>
            requires(sizeof...(strings) > 0)
        struct concatenate_pack_strings<separator, pack_string<>, strings...>
            : public concatenate_pack_strings<separator, strings...>
        {
                // Skip empty strings
        };

        template<typename separator, typename string, typename... strings>
            requires(sizeof...(strings) > 0)
        struct concatenate_pack_strings<separator, string, strings...>
        {
                using type = typename merge_pack_strings<
                    string,
                    separator,
                    typename concatenate_pack_strings<separator, strings...>::type>::type;
        };
    } // namespace __private

    /*
     * String unrolled as template parameters
     */
    template<char... data>
    struct pack_string
    {
            static constexpr auto size   = sizeof...(data);
            static constexpr auto string = []() {
                if constexpr (size > 0)
                {
                    return static_string<size> {std::array {data...}};
                }
                else
                {
                    return static_string {""};
                }
            }();

            template<typename... others>
            using merge =
                typename __private::merge_pack_strings<others...>::type;

            template<typename separator, typename... others>
            using concatenate =
                typename __private::concatenate_pack_strings<separator,
                                                             others...>::type;
    };

    template<typename... args>
    using concatenate_pack_strings_t =
        typename __private::concatenate_pack_strings<args...>::type;

    template<typename... args>
    using merge_pack_strings_t =
        typename __private::merge_pack_strings<args...>::type;

    /*
     * Build a pack string from a static string
     */
    template<static_string instance,
             typename indices = std::make_index_sequence<instance.size>>
    struct build_pack_string;

    template<static_string instance, std::size_t... indices>
    struct build_pack_string<instance, std::index_sequence<indices...>>
    {
            using type = pack_string<instance.data[indices]...>;
    };

    template<static_string instance>
    using build_pack_string_t =
        typename build_pack_string<instance>::type;
} // namespace e_regex

#endif /* E_REGEX_UTILITIES_PACK_STRING_HPP_*/
