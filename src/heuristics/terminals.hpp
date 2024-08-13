#ifndef E_REGEX_HEURISTICS_TERMINALS_HPP_
#define E_REGEX_HEURISTICS_TERMINALS_HPP_

#include "common.hpp"
#include "nodes.hpp"
#include "terminals/common.hpp"
#include "utilities/static_string.hpp"

namespace e_regex
{
    namespace _private
    {
        template<typename string, typename terminal = terminals::terminal<string>>
        concept exact = requires() { terminal::exact; };

        template<typename matchers, typename current = std::tuple<>>
        struct zip_matchers;

        template<typename prev, typename matcher, typename matcher1, typename... matchers>
            requires exact<matcher> && exact<matcher1>
        struct zip_matchers<std::tuple<matcher, matcher1, matchers...>, prev>
        {
                using matcher_ = merge_pack_strings_t<matcher, matcher1>;

                using type =
                    typename zip_matchers<std::tuple<matcher_, matchers...>,
                                          prev>::type;
        };

        template<typename... prev, typename matcher, typename matcher1, typename... matchers>
            requires(!exact<matcher> || !exact<matcher1>)
        struct zip_matchers<std::tuple<matcher, matcher1, matchers...>,
                            std::tuple<prev...>>
        {
                using type =
                    typename zip_matchers<std::tuple<matcher1, matchers...>,
                                          std::tuple<prev..., matcher>>::type;
        };

        template<typename... prev, typename matcher>
        struct zip_matchers<std::tuple<matcher>, std::tuple<prev...>>
        {
                using type = terminals::terminal<prev..., matcher>;
        };

        template<typename... prev>
        struct zip_matchers<std::tuple<>, std::tuple<prev...>>
        {
                using type = terminals::terminal<prev...>;
        };
    } // namespace _private

    /*
        Avoid repeating node parameters if there are only terminals
    */
    template<typename... string, typename... child_strings, typename... children>
    struct add_child<nodes::simple<terminals::terminal<string...>>,
                     nodes::simple<terminals::terminal<child_strings...>, children...>>
    {
            using zipped = typename _private::zip_matchers<
                std::tuple<string..., child_strings...>>::type;
            using type = nodes::simple<zipped, children...>;
    };
} // namespace e_regex

#endif /* E_REGEX_HEURISTICS_TERMINALS_HPP_*/
