#ifndef OPERATORS_ROUND_BRACKETS
#define OPERATORS_ROUND_BRACKETS

#include <utilities/extract_delimited_content.hpp>

#include "common.hpp"

namespace e_regex
{
    template<typename matcher>
    struct grouping_node
    {
            static constexpr std::size_t groups = 1 + matcher::groups;

            static constexpr auto match(auto result)
            {
                auto begin       = result.actual_iterator_end;
                auto match_index = result.matches;
                result.matches++;
                result = matcher::match(std::move(result));

                if (result)
                {
                    result.match_groups[match_index]
                        = std::string_view {begin, result.actual_iterator_end};
                }

                return result;
            }
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'('>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<grouping_node<subregex>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node,
                               std::tuple<pack_string<'('>, pack_string<'?'>, pack_string<':'>, tail...>>
    {
            // Non capturing group found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<subregex, std::tuple<>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_ROUND_BRACKETS */
