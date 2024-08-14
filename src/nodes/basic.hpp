#ifndef E_REGEX_NODES_BASIC_HPP_
#define E_REGEX_NODES_BASIC_HPP_

#include <concepts>
#include <type_traits>

#include "common.hpp"
#include "get_expression.hpp"
#include "match_result.hpp"
#include "terminals.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/static_string.hpp"

namespace e_regex::nodes
{
    template<typename T>
    concept valid_matcher
        = requires(match_result_data<T::groups, char> data) {
              {
                  T::template match<T>(data)
              } -> std::same_as<decltype(data)>;

              T::expression.get_view();
              T::admitted_first_chars;
          };

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

            using admitted_first_chars =
                typename extract_admission_set<matcher, children...>::type;

            template<typename... injected_children>
            using optimize
                = simple<typename matcher::template optimize<injected_children...>,
                         typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr auto match(auto res)
            {
                res = matcher::template match<children...>(res);

                return dfs<children...>(res);
            }
    };

    template<typename... children>
    struct simple<void, children...> : public base<void, children...>
    {
            static constexpr auto expression
                = get_children_expression<children...>();

            using admitted_first_chars =
                typename extract_admission_set<children...>::type;

            template<typename... injected_children>
            using optimize
                = simple<void, typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr auto match(auto res)
            {
                return dfs<children...>(res);
            }
    };

    template<typename child>
    struct simple<void, child> : public base<void, child>
    {
            static constexpr auto expression
                = get_children_expression<child>();

            using admitted_first_chars =
                typename extract_admission_set<child>::type;

            template<typename... injected_children>
            using optimize =
                typename child::template optimize<injected_children...>;

            template<typename... injected_children>
            static constexpr auto match(auto res)
            {
                return child::template match<injected_children...>(res);
            }
    };

    template<typename matcher>
    struct simple<matcher> : public base<matcher>
    {
            static constexpr auto expression = matcher::expression;

            using admitted_first_chars =
                typename extract_admission_set<matcher>::type;

            template<typename... injected_children>
            using optimize =
                typename matcher::template optimize<injected_children...>;

            template<typename... injected_children>
            static constexpr auto match(auto res)
            {
                return matcher::template match<>(res);
            }
    };

} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_BASIC_HPP_*/
