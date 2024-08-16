#ifndef E_REGEX_TERMINALS_SPACE_CHARACTERS_HPP_
#define E_REGEX_TERMINALS_SPACE_CHARACTERS_HPP_

#include <algorithm>
#include <array>

#include "common.hpp"
#include "utilities/admitted_set.hpp"

namespace e_regex::terminals
{
    using space_characters_admission_set
        = admitted_set<char, '\t', '\n', '\f', '\r', ' '>;

    template<>
    struct terminal<pack_string<'\\', 's'>>
        : public terminal_common<terminal<pack_string<'\\', 's'>>, space_characters_admission_set>
    {
            static constexpr auto expression = static_string {"\\s"};

            static constexpr auto match_(auto& result) -> auto&
            {
                constexpr std::array matched {
                    '\t', '\n', '\f', '\r', ' '};

                result = std::find(matched.begin(),
                                   matched.end(),
                                   *result.actual_iterator_end)
                         != matched.end();

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'S'>>
        : public negated_terminal<terminal<pack_string<'\\', 's'>>,
                                  space_characters_admission_set>
    {
            static constexpr auto expression = static_string {"\\S"};
    };
} // namespace e_regex::terminals

#endif /* E_REGEX_TERMINALS_SPACE_CHARACTERS_HPP_*/
