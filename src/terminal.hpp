#ifndef REGEX_NODE
#define REGEX_NODE

#include "static_string.hpp"

namespace e_regex
{
    template<typename identifier>
    struct terminal;

    template<char identifier>
    struct terminal<static_string<identifier>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                result = identifier == *query_iterator;
                return std::make_pair(std::move(result), query_iterator + 1);
            }
    };

    template<>
    struct terminal<static_string<'.'>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                return std::make_pair(std::move(result), query_iterator + 1);
            }
    };

    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<static_string<start>, static_string<end>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                static_assert(end >= start, "Range [a-b] must respect b >= a");

                result = *query_iterator >= start && *query_iterator <= end;

                return std::make_pair(std::move(result), query_iterator + 1);
            }
    };

    template<typename terminal>
    struct negated_terminal
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                auto res  = terminal::match(query_iterator, end, std::move(result));
                res.first = !res.first.is_accepted();

                return res;
            }
    };
}// namespace e_regex

#endif /* REGEX_NODE */
