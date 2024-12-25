#ifndef E_REGEX_UTILITIES_MACROS_HPP_
#define E_REGEX_UTILITIES_MACROS_HPP_

#if defined __forceinline
#    define EREGEX_ALWAYS_INLINE __forceinline
#elif defined __has_attribute
#    if __has_attribute(always_inline)
#        define EREGEX_ALWAYS_INLINE __attribute__((always_inline))
#    else
#        define EREGEX_ALWAYS_INLINE
#    endif
#else
#    define EREGEX_ALWAYS_INLINE
#endif


#endif /* E_REGEX_UTILITIES_MACROS_HPP_*/
