#ifndef E_REGEX_E_REGEX_HPP_
#define E_REGEX_E_REGEX_HPP_

#include "match_result.hpp"
#include "nodes/common.hpp"
#include "tree_builder.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/literal_string_view.hpp"
#include "utilities/pack_string.hpp"
#include "utilities/static_string.hpp"

namespace e_regex
{
    template<static_string expression>
    struct regex
    {
            using ast = typename tree_builder<
                build_pack_string_t<expression>>::tree::template optimize<>;

            static constexpr auto match(literal_string_view<> data)
            {
                return match_result<ast> {data};
            }

            constexpr auto operator()(literal_string_view<> data) const
            {
                return match(data);
            }

            static constexpr auto get_expression()
            {
                return ast::expression.to_view();
            }

            template<typename other>
            static constexpr auto is_independent() -> bool
            {
                using intersection = admitted_sets_intersection_t<
                    typename ast::admitted_first_chars,
                    typename other::ast::admitted_first_chars>;

                return intersection::empty;
            }

            template<typename T>
            static constexpr auto is_independent(T /*other*/) -> bool
            {
                return is_independent<T>();
            }

            static constexpr auto& groups
                = nodes::group_getter<ast>::value;
    };

} // namespace e_regex

#endif /* E_REGEX_E_REGEX_HPP_*/
