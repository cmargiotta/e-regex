#ifndef OPERATORS_HEX
#define OPERATORS_HEX

#include "common.hpp"
#include "static_string.hpp"
#include "utilities/extract_delimited_content.hpp"

namespace e_regex
{
    template<char C>
    concept hex = C >= '0' && C <= 'F';

    template<char... digits>
    struct hex_to_bin;

    template<char first, char second, char... tail>
    struct hex_to_bin<first, second, tail...>
    {
            static_assert(hex<first> && hex<second>, "Hex characters must be included between 0 and F");

            static constexpr char current = ((first - '0') << (4)) | (second - '0');

            using result
                = merge_pack_strings_t<pack_string<current>, typename hex_to_bin<tail...>::result>;
    };

    template<>
    struct hex_to_bin<>
    {
            using result = pack_string<>;
    };

    template<typename digits>
    struct hex_tuple_to_bin;

    template<char... digits>
    struct hex_tuple_to_bin<std::tuple<pack_string<digits...>>>
    {
            using result = typename hex_to_bin<digits...>::result;
    };

    template<typename last_node, char first_nibble, char second_nibble, typename... tail>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', 'x'>, pack_string<first_nibble>, pack_string<second_nibble>, tail...>>
    {
            using value = typename hex_to_bin<first_nibble, second_nibble>::result;

            using new_node =
                typename tree_builder_helper<basic_node<exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename hex_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<basic_node<exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_HEX */
