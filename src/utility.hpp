#ifndef UTILITY
#define UTILITY

#include <tuple>

#include "static_string.hpp"

namespace e_regex
{
    template<typename T1, typename T2>
    struct tuple_cat;

    template<typename... T1, typename... T2>
    struct tuple_cat<std::tuple<T1...>, std::tuple<T2...>>
    {
            using type = std::tuple<T1..., T2...>;
    };

    template<typename... T1, typename T2>
    struct tuple_cat<std::tuple<T1...>, T2>
    {
            using type = std::tuple<T1..., T2>;
    };

    template<typename T1, typename T2>
    using tuple_cat_t = typename tuple_cat<T1, T2>::type;

    template<char open, char closing, unsigned skip_counter, typename current, typename string>
    struct extract_delimited_content;

    template<char open, char closing, typename current, typename... tail_>
    struct extract_delimited_content<open, closing, 0, current, std::tuple<pack_string<closing>, tail_...>>
    {
            // Base case, closing delimiter found
            using result    = current;
            using remaining = std::tuple<tail_...>;
    };

    template<char open, char closing, typename current>
    struct extract_delimited_content<open, closing, 0, current, std::tuple<>>
    {
            // Base case, malformed token string
            using result    = std::tuple<>;
            using remaining = result;
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<pack_string<closing>, tail...>>
        : public extract_delimited_content<open,
                                           closing,
                                           skip_counter - 1,
                                           tuple_cat_t<current, pack_string<closing>>,
                                           std::tuple<tail...>>
    {
            // Closing delimiter found, but it has to be skipped
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<pack_string<open>, tail...>>
        : public extract_delimited_content<open,
                                           closing,
                                           skip_counter + 1,
                                           tuple_cat_t<current, pack_string<open>>,
                                           std::tuple<tail...>>
    {
            // Open delimiter found, increase skip counter
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename head, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<head, tail...>>
        : public extract_delimited_content<open, closing, skip_counter, tuple_cat_t<current, head>, std::tuple<tail...>>
    {
            // Iterate characters
    };

    template<char open, char closing, typename string>
    using extract_delimited_content_t
        = extract_delimited_content<open, closing, 0, std::tuple<>, string>;

    template<char separator, typename tokens, typename current = std::tuple<std::tuple<>>>
    struct split;

    template<char separator, typename... tail, typename... current_tokens, typename... currents>
    struct split<separator, std::tuple<pack_string<'|'>, tail...>, std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            using current = std::tuple<std::tuple<>, std::tuple<current_tokens...>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename head, typename... tail, typename... current_tokens, typename... currents>
    struct split<separator, std::tuple<head, tail...>, std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            using current = std::tuple<std::tuple<current_tokens..., head>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename current>
    struct split<separator, std::tuple<>, current>
    {
            using type = current;
    };

    template<char separator, typename tokens>
    using split_t = typename split<separator, tokens>::type;

    template<typename... T>
    struct last;

    template<typename head, typename... tail>
    struct last<head, tail...> : public last<tail...>
    {
    };

    template<typename head>
    struct last<head>
    {
            using type = head;
    };

    template<>
    struct last<>
    {
            using type = void;
    };

    template<typename... T>
    using last_t = typename last<T...>::type;

    template<typename current, typename... T>
    struct remove_last;

    template<typename... current, typename head, typename... tail>
    struct remove_last<std::tuple<current...>, head, tail...>
        : public remove_last<std::tuple<current..., head>, tail...>
    {
    };

    template<typename current, typename last>
    struct remove_last<current, last>
    {
            using type = current;
    };

    template<typename current>
    struct remove_last<current>
    {
            using type = void;
    };

    template<typename... T>
    using remove_last_t = typename remove_last<std::tuple<>, T...>::type;

}// namespace e_regex

#endif /* UTILITY */
