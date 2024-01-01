#ifndef OPERATORS_OCTAL
#define OPERATORS_OCTAL

#include "common.hpp"
#include "static_string.hpp"
#include "utilities/extract_delimited_content.hpp"

namespace e_regex
{
    template<char C>
    concept octal = C >= '0' && C <= '8';

    template<char... digits>
    struct octal_to_bin;

    template<char first, char second, char third, char... tail>
    struct octal_to_bin<first, second, third, tail...>
    {
            static_assert(octal<first> && octal<second> && octal<third>,
                          "Octal characters must be included between 0 and 8");

            static constexpr char current
                = ((first - '0') << 6) | ((second - '0') << 3) | (third - '0');

            using result
                = merge_pack_strings_t<pack_string<current>, typename octal_to_bin<tail...>::result>;
    };

    template<>
    struct octal_to_bin<>
    {
            using result = pack_string<>;
    };

    template<typename digits>
    struct octal_tuple_to_bin;

    template<char... digits>
    struct octal_tuple_to_bin<std::tuple<pack_string<digits...>>>
    {
            using result = typename octal_to_bin<digits...>::result;
    };

    template<typename last_node, char first_nibble, char second_nibble, char third_nibble, typename... tail>
        requires octal<first_nibble> && octal<second_nibble> && octal<third_nibble>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', first_nibble>, pack_string<second_nibble>, pack_string<third_nibble>, tail...>>
    {
            using value = typename octal_to_bin<first_nibble, second_nibble, third_nibble>::result;

            using new_node =
                typename tree_builder_helper<nodes::basic<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'o'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename octal_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<nodes::basic<terminals::exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_OCTAL */
