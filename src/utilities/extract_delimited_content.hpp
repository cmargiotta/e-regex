#ifndef UTILITIES_EXTRACT_DELIMITED_CONTENT
#define UTILITIES_EXTRACT_DELIMITED_CONTENT

#include <tuple>

#include <static_string.hpp>
#include <utilities/tuple_cat.hpp>

namespace e_regex
{
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
}// namespace e_regex

#endif /* UTILITIES_EXTRACT_DELIMITED_CONTENT */
