#ifndef E_REGEX_HPP
#define E_REGEX_HPP

#include "match_result.hpp"
#include "static_string.hpp"
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

    template<e_regex::tokenization::token_definition auto... matchers>
    constexpr auto tokenize = [](literal_string_view<> expression)
    {
        using data             = typename e_regex::tokenization::result_builder<matchers...>::data;
        auto matcher           = match<data::token_matcher::string>;
        auto separator_matcher = match<data::separator_matcher::string>;

        return tokenization::result<matcher, separator_matcher, data::types> {expression};
    };

    // template<static_string regex, static_string data, static_string separator = static_string
    // {""}, typename token_type = void> using token_t = tokenization::prebuilt_result<match<regex>,
    // match<separator>, data, token_type>;
}// namespace e_regex

#endif /* E_REGEX_HPP */
