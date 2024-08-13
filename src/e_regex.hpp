#ifndef E_REGEX_E_REGEX_HPP_
#define E_REGEX_E_REGEX_HPP_

#include "match_result.hpp"
#include "tree_builder.hpp"
#include "utilities/literal_string_view.hpp"
#include "utilities/pack_string.hpp"
#include "utilities/static_string.hpp"

namespace e_regex
{
    template<static_string expression>
    class regex
    {
        private:
            using ast =
                typename tree_builder<build_pack_string_t<expression>>::tree;

        public:
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

            static constexpr const auto& groups = ast::groups;
    };

} // namespace e_regex

#endif /* E_REGEX_E_REGEX_HPP_*/
