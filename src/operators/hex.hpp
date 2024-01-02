#ifndef OPERATORS_HEX
#define OPERATORS_HEX

#include "common.hpp"
#include "static_string.hpp"
#include "terminals.hpp"
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

            static constexpr char current = ((first - '0') << 4) | (second - '0');

            using result
                = merge_pack_strings_t<pack_string<current>, typename hex_to_bin<tail...>::result>;
    };

    template<char first, char second, char third>
    struct hex_to_bin<first, second, third>
    {
            static_assert(hex<first> && hex<second> && hex<third>,
                          "Hex characters must be included between 0 and F");

            static constexpr char current        = ((first - '0') << (4)) | (second - '0');
            static constexpr char current_second = (third - '0') << 4;

            using result = pack_string<current>;
    };

    template<char first>
    struct hex_to_bin<first>
    {
            static_assert(hex<first>, "Hex characters must be included between 0 and F");

            static constexpr char current = first - '0';

            using result = pack_string<current>;
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

    template<typename last_node, char first_nibble, char second_nibble, typename... tail, auto group_index>
        requires hex<first_nibble> && hex<second_nibble>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', 'x'>, pack_string<first_nibble>, pack_string<second_nibble>, tail...>,
        group_index>
    {
            using value = typename hex_to_bin<first_nibble, second_nibble>::result;

            using new_node =
                typename tree_builder_helper<nodes::simple<terminals::exact_matcher<value>>,
                                             std::tuple<tail...>,
                                             group_index>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, char nibble, typename... tail, auto group_index>
        requires hex<nibble>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<nibble>, tail...>, group_index>
    {
            using value = typename hex_to_bin<nibble>::result;

            using new_node =
                typename tree_builder_helper<nodes::simple<terminals::exact_matcher<value>>,
                                             std::tuple<tail...>,
                                             group_index>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail, auto group_index>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<'{'>, tail...>, group_index>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename hex_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<nodes::simple<terminals::exact_matcher<value>>,
                                             typename substring::remaining,
                                             group_index>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_HEX */
