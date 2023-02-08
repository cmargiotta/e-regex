#ifndef E_REGEX
#define E_REGEX

#include <string_view>

#include "match_result.hpp"
#include "static_string.hpp"
#include "tokenization_result.hpp"
#include "tree_builder.hpp"

namespace e_regex
{
    template<static_string regex>
    constexpr auto match = [](std::string_view expression)
    {
        using packed  = build_pack_string_t<regex>;
        using matcher = typename tree_builder<packed>::tree;

        return match_result<matcher> {expression};
    };

    template<static_string regex, static_string separator = static_string {".*"}>
    constexpr auto tokenize = [](std::string_view expression)
    {
        auto matcher           = match<regex>;
        auto separator_matcher = match<separator>;

        auto result = matcher(expression);

        return tokenization_result {result, separator_matcher};
    };
}// namespace e_regex

#endif /* E_REGEX */
