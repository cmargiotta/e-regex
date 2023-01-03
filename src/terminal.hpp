#ifndef REGEX_NODE
#define REGEX_NODE

#include <algorithm>
#include <array>
#include <utility>

#include "static_string.hpp"

namespace e_regex
{
    template<typename identifier>
    struct exact_matcher;

    template<char identifier>
    struct exact_matcher<static_string<identifier>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                result = identifier == *query_iterator;

                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename... identifier>
    struct terminal;

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
    };

    template<>
    struct terminal<static_string<'\\', 'w'>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                result = (*query_iterator >= 'A' && *query_iterator <= 'Z')
                         || (*query_iterator >= 'a' && *query_iterator <= 'z');

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<static_string<'\\', 's'>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                static constexpr std::array matched {' ', '\t', '\n', '\r', '\f'};

                result = std::find(matched.begin(), matched.end(), *query_iterator) != matched.end();

                result.actual_iterator_end++;

                return result;
            }
    };

    template<char identifier>
    struct terminal<static_string<'\\', identifier>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator, auto result)
            {
                result = *query_iterator == identifier;

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<static_string<'.'>>
    {
            template<typename Iterator>
            static constexpr auto match(Iterator, Iterator, auto result)
            {
                result.actual_iterator_end++;
                return result;
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

                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename terminal>
    struct negated_terminal
    {
            template<typename Iterator>
            static constexpr auto match(Iterator query_iterator, Iterator end, auto result)
            {
                result = terminal::match(query_iterator, end, std::move(result));
                result = !result.accepted;

                return result;
            }
    };
}// namespace e_regex

#endif /* REGEX_NODE */
