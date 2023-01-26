#ifndef OPERATORS_HEX
#define OPERATORS_HEX

#include "common.hpp"

namespace e_regex
{
    template<typename last_node, char first_nibble, char second_nibble, typename... tail>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', '0'>, pack_string<first_nibble>, pack_string<second_nibble>, tail...>>
    {
            static_assert(first_nibble >= '0' && first_nibble <= 'F' && second_nibble >= '0'
                              && second_nibble <= 'F',
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
