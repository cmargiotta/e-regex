#ifndef OPERATORS_OCTAL
#define OPERATORS_OCTAL

#include "common.hpp"

namespace e_regex
{
    template<char C>
    concept octal = C >= '0' && C <= '8';

    template<typename last_node, char first_nibble, char second_nibble, char third_nibble, typename... tail>
    requires octal<first_nibble> && octal<second_nibble> && octal<third_nibble>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', first_nibble>, pack_string<second_nibble>, pack_string<third_nibble>, tail...>>
    {
            static consteval auto octal_to_bin(char v) -> char
            {
                return v - '0';
            }

            using value
                = pack_string<(octal_to_bin(first_nibble) << 6) | (octal_to_bin(second_nibble) << 3)
                              | octal_to_bin(third_nibble)>;

            using new_node =
                typename tree_builder_helper<basic_node<exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_OCTAL */
