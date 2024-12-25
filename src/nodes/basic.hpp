#ifndef E_REGEX_NODES_BASIC_HPP_
#define E_REGEX_NODES_BASIC_HPP_

#include <concepts>
#include <tuple>
#include <type_traits>

#include "common.hpp"
#include "get_expression.hpp"
#include "meta.hpp"
#include "terminals.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/macros.hpp"
#include "utilities/math.hpp"
#include "utilities/static_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher>
    struct admitted_first_chars_getter
    {
            using type = typename matcher::admitted_first_chars;
    };

    template<>
    struct admitted_first_chars_getter<void>
    {
            using type = admitted_set<char>;
    };

    template<typename matcher, typename... children>
    struct simple : public base<matcher, children...>
    {
            static constexpr auto expression = []() {
                if constexpr (sizeof...(children) <= 1)
                {
                    return matcher::expression
                           + get_children_expression<children...>();
                }
                else
                {
                    return matcher::expression + "(?:"
                           + get_children_expression<children...>()
                           + ')';
                }
            }();

            using admission_set = std::conditional_t<
                matcher::meta.minimum_match_size == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename extract_admission_set<matcher>::type>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = policy::NONE,
                .minimum_match_size
                = matcher::meta.minimum_match_size
                  + min(children::meta.minimum_match_size...),
                .maximum_match_size
                = matcher::meta.maximum_match_size
                  + max(children::meta.minimum_match_size...),
            };

            template<typename... injected_children>
            using optimize
                = simple<typename matcher::template optimize<injected_children...>,
                         typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto match(auto& res) -> auto&
            {
                auto begin = res.actual_iterator_end;

                matcher::match(res);

                if (res)
                {
                    if (!dfs<std::tuple<children...>,
                             std::tuple<injected_children...>>(res))
                    {
                        res.actual_iterator_end = begin;
                    }
                }

                return res;
            }
    };

    template<typename... children>
    struct simple<void, children...> : public base<void, children...>
    {
            using child
                = std::tuple_element_t<0, std::tuple<children...>>;

            static constexpr auto expression
                = get_children_expression<children...>();

            using admission_set = std::conditional_t<
                child::meta.minimum_match_size == 0,
                typename extract_admission_set<children...>::type,
                typename extract_admission_set<child>::type>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = policy::NONE,
                .minimum_match_size
                = min(children::meta.minimum_match_size...),
                .maximum_match_size
                = max(children::meta.minimum_match_size...),
            };

            template<typename... injected_children>
            using optimize
                = simple<void, typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto match(auto& res) -> auto&
            {
                return dfs<std::tuple<children...>,
                           std::tuple<injected_children...>>(res);
            }
    };

    template<typename child>
    struct simple<void, child> : public base<void, child>
    {
            static constexpr auto expression
                = get_children_expression<child>();

            using admission_set =
                typename extract_admission_set<child>::type;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_            = policy::NONE,
                .minimum_match_size = child::meta.minimum_match_size,
                .maximum_match_size = child::meta.minimum_match_size,
            };

            template<typename... injected_children>
            using optimize =
                typename child::template optimize<injected_children...>;

            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto match(auto& res) -> auto&
            {
                return child::template match<injected_children...>(res);
            }
    };

    template<typename matcher>
    struct simple<matcher> : public base<matcher>
    {
            static constexpr auto expression = matcher::expression;

            using admission_set =
                typename extract_admission_set<matcher>::type;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = policy::NONE,
                .minimum_match_size = matcher::meta.minimum_match_size,
                .maximum_match_size = matcher::meta.maximum_match_size,
            };

            template<typename... injected_children>
            using optimize =
                typename matcher::template optimize<injected_children...>;

            template<typename... injected_children>
            static constexpr EREGEX_ALWAYS_INLINE auto match(auto& res) -> auto&
            {
                return matcher::match(res);
            }
    };

} // namespace e_regex::nodes


#endif /* E_REGEX_NODES_BASIC_HPP_*/
