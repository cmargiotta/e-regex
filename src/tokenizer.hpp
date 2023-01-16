#ifndef TOKENIZER
#define TOKENIZER

#include <tuple>

#include "static_string.hpp"
#include "utilities/tuple_cat.hpp"

namespace e_regex
{
    template<typename matches, char... string>
    struct extract_braces_numbers;

    template<char... current, char head, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, head, tail...>
        : public extract_braces_numbers<pack_string<current..., head>, tail...>
    {
            // Populating first number
    };

    template<typename first, char... tail>
    struct extract_braces_numbers<std::tuple<first>, '-', tail...>
        : public extract_braces_numbers<std::tuple<first, pack_string<'-'>>, tail...>
    {
            // '-' found, first number done
    };

    template<char... current, typename first, typename second, char head, char... tail>
    struct extract_braces_numbers<std::tuple<first, second, pack_string<current...>>, head, tail...>
        : public extract_braces_numbers<std::tuple<first, second, pack_string<current..., head>>, tail...>
    {
            // Populating second number
    };

    template<typename match, char... tail>
    struct extract_braces_numbers<match, '}', tail...>
    {
            // '}' found, finishing
            using numbers   = match;
            using remaining = pack_string<tail...>;
    };

    template<char... string>
    using extract_braces_numbers_t = extract_braces_numbers<std::tuple<pack_string<>>, string...>;

    template<typename current, typename string>
    struct tokenizer;

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<head, tail...>>
    {
            // Simple iteration
            using current_ = tuple_cat_t<current, std::tuple<pack_string<head>>>;
            using tokens   = typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<'\\', head, tail...>>
    {
            // Escaped character
            using current_ = tuple_cat_t<current, std::tuple<pack_string<'\\', head>>>;
            using tokens   = typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char... tail>
    struct tokenizer<current, pack_string<'{', tail...>>
    {
            // Number inside braces
            using numbers  = extract_braces_numbers_t<tail...>;
            using current_ = tuple_cat_t<current, typename numbers::numbers>;
            using tokens   = typename tokenizer<current_, typename numbers::remaining>::tokens;
    };

    template<typename current>
    struct tokenizer<current, pack_string<>>
    {
            // Base case
            using tokens = current;
    };

    template<typename string>
    using tokenize = typename tokenizer<std::tuple<>, string>::tokens;
}// namespace e_regex

#endif /* TOKENIZER */
