#ifndef E_REGEX_TOKENIZER_HPP_
#define E_REGEX_TOKENIZER_HPP_

#include <tuple>

#include "utilities/static_string.hpp"
#include "utilities/tuple_cat.hpp"

namespace e_regex
{
    template<typename matches, char... string>
    struct extract_braces_numbers;

    template<char... current, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, ',', tail...>
        : public extract_braces_numbers<
              tuple_cat_t<std::tuple<pack_string<current...>>,
                          std::tuple<pack_string<','>>>,
              tail...>
    {
            // ',' found, first number done
    };

    template<char... current, char head, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, head, tail...>
        : public extract_braces_numbers<std::tuple<pack_string<current..., head>>,
                                        tail...>
    {
            // Populating first number
    };

    // Probably gcc bug -> ambiguous template instantiation if compact
    template<typename first, typename second, char... current, char... tail>
    struct extract_braces_numbers<std::tuple<first, second, pack_string<current...>>,
                                  '}',
                                  tail...>
    {
            // '}' found, finishing
            using numbers   = std::tuple<pack_string<'{'>,
                                       first,
                                       second,
                                       pack_string<current...>,
                                       pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<typename first, typename second, char... tail>
    struct extract_braces_numbers<std::tuple<first, second>, '}', tail...>
    {
            // '}' found, finishing
            using numbers
                = std::tuple<pack_string<'{'>, first, second, pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<char... current, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, '}', tail...>
    {
            // '}' found, finishing
            using numbers   = std::tuple<pack_string<'{'>,
                                       pack_string<current...>,
                                       pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<typename first, typename second, char head, char... tail>
    struct extract_braces_numbers<std::tuple<first, second>, head, tail...>
        : public extract_braces_numbers<std::tuple<first, second, pack_string<head>>,
                                        tail...>
    {
            // Populating second number
    };

    template<char... current, typename first, typename second, char head, char... tail>
    struct extract_braces_numbers<std::tuple<first, second, pack_string<current...>>,
                                  head,
                                  tail...>
        : public extract_braces_numbers<
              std::tuple<first, second, pack_string<current..., head>>,
              tail...>
    {
            // Populating second number
    };

    template<char... string>
    using extract_braces_numbers_t
        = extract_braces_numbers<std::tuple<pack_string<>>, string...>;

    template<typename current, typename string>
    struct tokenizer;

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<head, tail...>>
    {
            // Simple iteration
            using current_
                = tuple_cat_t<current, std::tuple<pack_string<head>>>;
            using tokens =
                typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<'\\', head, tail...>>
    {
            // Escaped character
            using current_
                = tuple_cat_t<current, std::tuple<pack_string<'\\', head>>>;
            using tokens =
                typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char... tail>
    struct tokenizer<current, pack_string<'{', tail...>>
    {
            // Number inside braces
            using numbers = extract_braces_numbers_t<tail...>;
            using current_
                = tuple_cat_t<current, typename numbers::numbers>;
            using tokens =
                typename tokenizer<current_, typename numbers::remaining>::tokens;
    };

    template<typename current>
    struct tokenizer<current, pack_string<>>
    {
            // Base case
            using tokens = current;
    };

    template<typename string>
    using tokenizer_t =
        typename tokenizer<std::tuple<>, string>::tokens;
} // namespace e_regex

#endif /* E_REGEX_TOKENIZER_HPP_*/
