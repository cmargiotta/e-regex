#ifndef E_REGEX_UTILITIES_FIRST_TYPE_HPP_
#define E_REGEX_UTILITIES_FIRST_TYPE_HPP_

#include <tuple>

namespace e_regex
{
    template<typename data>
    struct first_type;

    template<typename head, typename... tail>
    struct first_type<std::tuple<head, tail...>>
    {
            using type      = head;
            using remaining = std::tuple<tail...>;
    };

    template<>
    struct first_type<std::tuple<>>
    {
            using type      = void;
            using remaining = std::tuple<>;
    };

    template<typename data>
    using first_type_t = typename first_type<data>::type;

} // namespace e_regex

#endif /* E_REGEX_UTILITIES_FIRST_TYPE_HPP_*/
