#ifndef E_REGEX_UTILITIES_CONCEPTS_HPP_
#define E_REGEX_UTILITIES_CONCEPTS_HPP_

#include <tuple>
#include <type_traits>

namespace e_regex
{
    namespace __private
    {
        template<typename T>
        struct is_tuple : public std::false_type
        {
        };

        template<typename... T>
        struct is_tuple<std::tuple<T...>> : public std::true_type
        {
        };
    }// namespace __private

    template<typename T>
    concept tuple = __private::is_tuple<T>::value;
}// namespace e_regex

#endif /* E_REGEX_UTILITIES_CONCEPTS_HPP_*/
