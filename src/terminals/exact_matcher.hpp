#ifndef TERMINALS_EXACT_MATCHER_HPP
#define TERMINALS_EXACT_MATCHER_HPP

#include "common.hpp"
#include "static_string.hpp"

namespace e_regex::terminals
{
    namespace _private
    {
        constexpr auto exact_match(char identifier, auto result)
        {
            result = identifier == *result.actual_iterator_end;
            result.actual_iterator_end++;

            return result;
        }
    }// namespace _private

    template<typename identifier>
    struct exact_matcher;

    template<char identifier>
    struct exact_matcher<pack_string<identifier>>
        : public terminal_common<exact_matcher<pack_string<identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                return _private::exact_match(identifier, std::move(result));
            }
    };

    template<char... identifier>
    struct exact_matcher<pack_string<identifier...>>
        : public terminal_common<exact_matcher<pack_string<identifier...>>>
    {
            static constexpr auto match_(auto result)
            {
                for (auto c: pack_string<identifier...>::string.to_view())
                {
                    result = c == *result.actual_iterator_end;
                    result.actual_iterator_end++;

                    if (!result)
                    {
                        break;
                    }
                }

                return result;
            }
    };

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
            static constexpr bool exact = true;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_EXACT_MATCHER_HPP */
