#ifndef OPERATORS_HEX
#define OPERATORS_HEX

#include "common.hpp"

namespace e_regex
{
    template<char C>
    concept hex = C >= '0' && C <= 'F';

    template<typename last_node, char first_nibble, char second_nibble, typename... tail>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', 'x'>, pack_string<first_nibble>, pack_string<second_nibble>, tail...>>
    {
            static_assert(hex<first_nibble> && hex<second_nibble>,
                          "Hex characters must be included between 0 and F");

            static consteval auto hex_to_bin(char v) -> char
            {
                return v - '0';
            }

            using value = pack_string<(hex_to_bin(first_nibble) << 4) | hex_to_bin(second_nibble)>;

            using new_node =
                typename tree_builder_helper<basic_node<exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_HEX */
