#ifndef TOKENIZER
#define TOKENIZER

#include <tuple>

#include "static_string.hpp"
#include "utility.hpp"

namespace e_regex
{

    template<typename current, typename string>
    struct tokenizer;

    template<typename current, char head, char... tail>
    struct tokenizer<current, static_string<head, tail...>>
    {
            // Simple iteration
            using current_ = tuple_cat_t<current, std::tuple<static_string<head>>>;
            using tokens   = typename tokenizer<current_, static_string<tail...>>::tokens;
    };

    template<typename current, char head, char... tail>
    struct tokenizer<current, static_string<'\\', head, tail...>>
    {
            // Escaped character
            using current_ = tuple_cat_t<current, std::tuple<static_string<'\\', head>>>;
            using tokens   = typename tokenizer<current_, static_string<tail...>>::tokens;
    };

    template<typename current>
    struct tokenizer<current, static_string<>>
    {
            // Base case
            using tokens = current;
    };

    template<typename string>
    using tokenize = typename tokenizer<std::tuple<>, string>::tokens;
}// namespace e_regex

#endif /* TOKENIZER */
