#ifndef TERMINALS_COMMON_HPP
#define TERMINALS_COMMON_HPP

#include "static_string.hpp"

namespace e_regex::terminals
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

    template<typename... identifiers>
    struct terminal;

    // Avoids instantiating nodes code for consecutive matchers
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

    template<typename terminal>
    struct rebuild_expression;

    template<>
    struct rebuild_expression<terminal<>>
    {
            using string = pack_string<>;
    };

    template<char... chars, typename... tail>
    struct rebuild_expression<terminal<pack_string<chars...>, tail...>>
    {
            using string
                = merge_pack_strings_t<pack_string<chars...>,
                                       typename rebuild_expression<terminal<tail...>>::string>;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_COMMON_HPP */
