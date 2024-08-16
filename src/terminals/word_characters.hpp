#ifndef E_REGEX_TERMINALS_WORD_CHARACTERS_HPP_
#define E_REGEX_TERMINALS_WORD_CHARACTERS_HPP_

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    using word_characters_admission_set = merge_admitted_sets_t<
        merge_admitted_sets_t<admitted_set_range_t<char, 'A', 'Z'>,
                              admitted_set_range_t<char, 'a', 'z'>>,
        merge_admitted_sets_t<admitted_set_range_t<char, '0', '9'>,
                              admitted_set<char, '_'>>>;

    template<>
    struct terminal<pack_string<'\\', 'w'>>
        : public terminal_common<terminal<pack_string<'\\', 'w'>>, word_characters_admission_set>
    {
            static constexpr auto expression = static_string {"\\w"};

            static constexpr __attribute__((always_inline)) auto
                match_(auto& result) -> auto&
            {
                const auto& current = *result.actual_iterator_end;

                result = (current >= 'A' && current <= 'Z')
                         || (current >= 'a' && current <= 'z')
                         || (current >= '0' && current <= '9')
                         || (current == '_');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'W'>>
        : public negated_terminal<terminal<pack_string<'\\', 'w'>>, word_characters_admission_set>
    {
            static constexpr auto expression = static_string {"\\W"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_WORD_CHARACTERS_HPP_*/
