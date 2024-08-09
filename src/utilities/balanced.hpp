#ifndef UTILITIES_BALANCED_HPP
#define UTILITIES_BALANCED_HPP

#include "static_stack.hpp"
#include "static_string.hpp"

namespace e_regex
{
    template<is_static_stack<char> stack, typename... strings>
    struct balanced : public std::false_type
    {
            // Base negative case, did not match any positive case
    };

    template<typename stack>
        requires(stack::empty)
    struct balanced<stack> : public std::true_type
    {
            // Base positive case, empty stack and no more tokens
    };

    template<typename stack, typename string, typename... strings>
    struct balanced<stack, string, strings...> : public balanced<stack, strings...>
    {
            // This token is not a parentheses
    };

    template<typename stack, typename... strings>
    struct balanced<stack, pack_string<'('>, strings...>
        : public balanced<typename stack::template push<'('>, strings...>
    {
            // ( found
    };

    template<typename stack, typename... strings>
        requires(!stack::empty && stack::head == '(')
    struct balanced<stack, pack_string<')'>, strings...>
        : public balanced<typename stack::pop, strings...>
    {
            // ) found and a ( is on top of the stack
    };

    template<typename stack, typename... strings>
    struct balanced<stack, pack_string<'['>, strings...>
        : public balanced<typename stack::template push<'['>, strings...>
    {
            // [ found
    };

    template<typename stack, typename... strings>
        requires(!stack::empty && stack::head == '[')
    struct balanced<stack, pack_string<']'>, strings...>
        : public balanced<typename stack::pop, strings...>
    {
            // ] found and a [ is on top of the stack
    };

    template<typename stack, typename... strings>
    struct balanced<stack, pack_string<'{'>, strings...>
        : public balanced<typename stack::template push<'{'>, strings...>
    {
            // { found
    };

    template<typename stack, typename... strings>
        requires(!stack::empty && stack::head == '{')
    struct balanced<stack, pack_string<'}'>, strings...>
        : public balanced<typename stack::pop, strings...>
    {
            // } found and a { is on top of the stack
    };

    template<typename... strings>
    static constexpr auto balanced_v = balanced<static_stack<char>, strings...>::value;

}// namespace e_regex

#endif /* UTILITIES_BALANCED_HPP */
