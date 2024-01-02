#ifndef E_REGEX_HPP
#define E_REGEX_HPP

#include "match_result.hpp"
#include "static_string.hpp"
#include "tokenization/prebuilt_result.hpp"
#include "tokenization/result.hpp"
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

    template<static_string regex, static_string separator = static_string {""}, typename token_type = void>
    constexpr auto tokenize = [](literal_string_view<> expression)
    {
        auto matcher           = match<regex>;
        auto separator_matcher = match<separator>;

        return tokenization::result<matcher, separator_matcher, token_type> {expression};
    };

    template<static_string regex, static_string data, static_string separator = static_string {""}, typename token_type = void>
    using token_t = tokenization::prebuilt_result<match<regex>, match<separator>, data, token_type>;
}// namespace e_regex

#endif /* E_REGEX_HPP */
