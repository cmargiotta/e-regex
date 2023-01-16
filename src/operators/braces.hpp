#ifndef OPERATORS_BRACES
#define OPERATORS_BRACES

#include <limits>

#include "common.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/pack_string_to_number.hpp"

namespace e_regex
{
    template<typename matcher, std::size_t begin, std::size_t end = std::numeric_limits<std::size_t>::max()>
    struct quantified_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                unsigned matches = 0;

                auto last_res = result;

                while (last_res.actual_iterator_end < result.query.end())
                {
                    last_res.matches = result.matches;// Only last group is considered
                    auto res         = matcher::match(last_res);

                    if (res)
                    {
                        last_res = std::move(res);
                        matches++;
                    }
                    else
                    {
                        break;
                    }
                }

                result = std::move(last_res);
                result = matches >= begin && matches <= end;

                return result;
            }
    };

    template<typename matcher, typename data>
    struct quantified_node_builder;

    template<typename matcher, typename first>
    struct quantified_node_builder<matcher, std::tuple<first>>
        : public quantified_node<matcher, pack_string_to_number<first>::value>
    {
    };

    template<typename matcher, typename first, typename second>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<'-'>, second>>
        : public quantified_node<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value>
    {
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using new_node = basic_node<quantified_node_builder<last_node, substring>>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES */
