#ifndef REGEX_NODE
#define REGEX_NODE

#include <algorithm>
#include <array>
#include <utility>

#include "static_string.hpp"

namespace e_regex
{
    template<typename terminal>
    struct terminal_common
    {
            static constexpr auto match(auto result)
            {
                if (result.actual_iterator_end >= result.query.end())
                {
                    result = false;
                }
                else
                {
                    return terminal::match_(std::move(result));
                }

                return result;
            }
    };

    template<typename identifier>
    struct exact_matcher;

    template<char identifier>
    struct exact_matcher<pack_string<identifier>>
        : public terminal_common<exact_matcher<pack_string<identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                result = identifier == *result.actual_iterator_end;
                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename... identifiers>
    struct terminal;

    template<typename head, typename... tail>
    struct terminal<head, tail...>
    {
            static constexpr auto match(auto result)
            {
                result = terminal<head>::match(std::move(result));

                if (result)
                {
                    return terminal<tail...>::match(std::move(result));
                }

                return result;
            }
    };

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
    };

    template<>
    struct terminal<pack_string<'\\', 'w'>> : public terminal_common<terminal<pack_string<'\\', 'w'>>>
    {
            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= 'A' && *current <= 'Z') || (*current >= 'a' && *current <= 'z');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'd'>> : public terminal_common<terminal<pack_string<'\\', 'd'>>>
    {
            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= '0' && *current <= '9');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 's'>> : public terminal_common<terminal<pack_string<'\\', 's'>>>
    {
            static constexpr auto match_(auto result)
            {
                static constexpr std::array matched {' ', '\t', '\n', '\r', '\f'};

                result = std::find(matched.begin(), matched.end(), *result.actual_iterator_end)
                         != matched.end();

                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename terminal>
    struct negated_terminal
    {
            static constexpr auto match(auto result)
            {
                result = terminal::match_(std::move(result));
                result = !result.accepted;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'S'>> : public negated_terminal<terminal<pack_string<'\\', 's'>>>
    {
    };

    template<>
    struct terminal<pack_string<'\\', 'D'>> : public negated_terminal<terminal<pack_string<'\\', 'd'>>>
    {
    };

    template<>
    struct terminal<pack_string<'\\', 'W'>> : public negated_terminal<terminal<pack_string<'\\', 'w'>>>
    {
    };

    template<char identifier>
    struct terminal<pack_string<'\\', identifier>>
        : public terminal_common<terminal<pack_string<'\\', identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end == identifier;

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'.'>> : public terminal_common<terminal<pack_string<'.'>>>
    {
            static constexpr auto match_(auto result)
            {
                result.actual_iterator_end++;

                return result;
            }
    };

    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<pack_string<start>, pack_string<end>>
        : public terminal_common<range_terminal<pack_string<start>, pack_string<end>>>
    {
            static constexpr auto match(auto result)
            {
                static_assert(end >= start, "Range [a-b] must respect b >= a");

                auto current = result.actual_iterator_end;

                result = *current >= start && *current <= end;
                result.actual_iterator_end++;

                return result;
            }
    };

}// namespace e_regex

#endif /* REGEX_NODE */
