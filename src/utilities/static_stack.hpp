#ifndef UTILITIES_STATIC_STACK
#define UTILITIES_STATIC_STACK

#include <type_traits>
namespace e_regex
{
    template<typename T, T... data>
    struct static_stack
    {
            template<T d>
            using push = static_stack<T, d, data...>;

            static constexpr bool empty = true;
    };

    template<typename T, T last, T... data>
    struct static_stack<T, last, data...>
    {
            template<T d>
            using push = static_stack<T, d, last, data...>;

            using pop = static_stack<T, data...>;

            static constexpr bool        empty = false;
            static constexpr const auto &head  = last;
    };

    template<typename Data, typename T>
    struct is_static_stack_ : public std::false_type
    {
    };

    template<typename T, T... data>
    struct is_static_stack_<T, static_stack<T, data...>> : public std::true_type
    {
    };

    template<typename T, typename Data>
    concept is_static_stack = is_static_stack_<Data, T>::value;

}// namespace e_regex

#endif
