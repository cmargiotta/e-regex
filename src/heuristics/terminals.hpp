#ifndef HEURISTICS_TERMINALS_HPP
#define HEURISTICS_TERMINALS_HPP

#include <nodes.hpp>
#include <static_string.hpp>
#include <terminals/common.hpp>

#include "common.hpp"

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

                using type = typename zip_matchers<std::tuple<matcher_, matchers...>, prev>::type;
        };

        template<typename... prev, typename matcher, typename matcher1, typename... matchers>
            requires(!exact<matcher> || !exact<matcher1>)
        struct zip_matchers<std::tuple<matcher, matcher1, matchers...>, std::tuple<prev...>>
        {
                using type =
                    typename zip_matchers<std::tuple<matcher1, matchers...>, std::tuple<prev..., matcher>>::type;
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
    }// namespace _private

    /*
        Avoid repeating node parameters if there are only terminals
    */
    template<typename... string, typename... child_strings, typename... children, nodes::policy repetition_policy>
    struct add_child<
        nodes::basic_node<terminals::terminal<string...>, std::tuple<>, 1, 1, repetition_policy, false>,
        nodes::basic_node<terminals::terminal<child_strings...>, std::tuple<children...>, 1, 1, repetition_policy, false>>
    {
            using zipped =
                typename _private::zip_matchers<std::tuple<string..., child_strings...>>::type;
            using type
                = nodes::basic_node<zipped, std::tuple<children...>, 1, 1, repetition_policy, false>;
    };
}// namespace e_regex

#endif /* HEURISTICS_TERMINALS_HPP */
