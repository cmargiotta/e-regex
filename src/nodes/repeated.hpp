#ifndef E_REGEX_NODES_REPEATED_HPP_
#define E_REGEX_NODES_REPEATED_HPP_

#include <cstddef>

#include "basic.hpp"
#include "meta.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher, unsigned repetitions, typename... children>
    struct repeated : public base<matcher, children...>
    {
            using admission_set =
                typename extract_admission_set<matcher>::type;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = e_regex::policy::NONE,
                .minimum_match_size
                = matcher::meta.minimum_match_size * repetitions,
                .maximum_match_size
                = matcher::meta.maximum_match_size * repetitions,
            };

            static constexpr auto expression = []() {
                auto self = matcher::expression + '{'
                            + number_to_pack_string_t<repetitions>::string
                            + '}';

                if constexpr (sizeof...(children) <= 1)
                {
                    return self + get_children_expression<children...>();
                }
                else
                {
                    return self + "(?:"
                           + get_children_expression<children...>()
                           + ')';
                }
            }();

            template<typename... injected_children>
            using optimize
                = repeated<typename matcher::template optimize<>,
                           repetitions,
                           typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr auto match(auto& res) -> auto&
            {
                for (unsigned i = 0; i < repetitions; ++i)
                {
                    if (!matcher::match(res))
                    {
                        return res;
                    }
                }

                return dfs<std::tuple<children...>>(res);
            }
    };
} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_REPEATED_HPP_*/
