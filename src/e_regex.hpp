#ifndef E_REGEX
#define E_REGEX

#include "match_result.hpp"
#include "static_string.hpp"
#include "tokenization_result.hpp"
#include "tree_builder.hpp"
#include "utilities/literal_string_view.hpp"

namespace e_regex
{
    template<static_string regex>
    constexpr auto match = [](literal_string_view<> expression)
    {
        using packed  = build_pack_string_t<regex>;
        using matcher = typename tree_builder<packed>::tree;

        return match_result<matcher> {expression};
    };

    template<static_string regex, static_string separator = static_string {".*"}>
    constexpr auto tokenize = [](literal_string_view<> expression)
    {
        auto matcher           = match<regex>;
        auto separator_matcher = match<separator>;

        return tokenization_result {matcher(expression), separator_matcher};
    };

    template<static_string regex, static_string data, static_string separator = static_string {".*"}>
    using token_t
        = prebuilt_tokenization_result<match<regex>, match<separator>, static_cast<literal_string_view<char>>(data)>;
}// namespace e_regex

#endif /* E_REGEX */
