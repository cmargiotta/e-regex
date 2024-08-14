#ifndef E_REGEX_TERMINALS_COMMON_HPP_
#define E_REGEX_TERMINALS_COMMON_HPP_

#include <utility>

#include "utilities/admitted_set.hpp"
#include "utilities/static_string.hpp"

namespace e_regex::terminals
{
    template<typename terminal>
    struct terminal_common
    {
            template<typename... injected_children>
            using optimize = terminal;

            // Template used only for compatibility with nodes
            template<typename... injected_children>
            static constexpr auto match(auto result)
            {
                if (result.actual_iterator_end >= result.query.end())
                {
                    result = false;
                }
                else if (result)
                {
                    return terminal::match_(result);
                }

                return result;
            }
    };

    template<typename terminal>
    struct negated_terminal
    {
            using admitted_first_chars
                = admitted_set_complement_t<typename terminal::admitted_first_chars>;

            template<typename... injected_children>
            using optimize = negated_terminal;

            template<typename... injected_children>
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
            static constexpr auto expression
                = terminal<head>::expression
                  + (terminal<tail>::expression + ...);
            using admitted_first_chars =
                typename terminal<head>::admitted_first_chars;

            template<typename... injected_children>
            using optimize = terminal;

            template<typename... injected_children>
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

} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_COMMON_HPP_*/
