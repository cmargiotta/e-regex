#ifndef E_REGEX_TERMINALS_COMMON_HPP_
#define E_REGEX_TERMINALS_COMMON_HPP_

#include <utility>

#include "nodes/meta.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/macros.hpp"
#include "utilities/static_string.hpp"

namespace e_regex::terminals
{
    template<typename terminal, typename admission_set>
    struct terminal_common
    {
            template<typename... injected_children>
            using optimize = terminal;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_            = e_regex::policy::NONE,
                .minimum_match_size = 1,
                .maximum_match_size = 1,
            };

            // Template used only for compatibility with nodes
            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto
                match(auto& result) -> auto&
            {
                if (result.actual_iterator_end >= result.query.end())
                {
                    result.accepted = false;
                    return result;
                }

                return terminal::match_(result);
            }
    };

    template<typename terminal, typename admission_set>
    struct negated_terminal
        : public terminal_common<negated_terminal<terminal, admission_set>,
                                 admitted_set_complement_t<admission_set>>
    {
            template<typename... injected_children>
            using optimize = negated_terminal;

            template<typename... injected_children>
            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                result = terminal::match_(std::move(result));
                result = !result.accepted;

                return result;
            }
    };

    template<typename... identifiers>
    struct terminal;
} // namespace e_regex::terminals


#endif /* E_REGEX_TERMINALS_COMMON_HPP_*/
