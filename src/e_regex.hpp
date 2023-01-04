#ifndef E_REGEX
#define E_REGEX

#include <string_view>

#include "tree_builder.hpp"

namespace e_regex
{
    template<static_string regex>
    constexpr auto match = [](std::string_view expression)
    {
        using packed  = build_pack_string_t<regex>;
        using matcher = typename tree_builder<packed>::tree;

        return match_result<matcher> {expression};
    };
}// namespace e_regex

#endif /* E_REGEX */
