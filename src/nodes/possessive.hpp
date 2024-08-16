#ifndef E_REGEX_NODES_POSSESSIVE_HPP_
#define E_REGEX_NODES_POSSESSIVE_HPP_

#include <cstddef>
#include <limits>

#include "basic.hpp"
#include "meta.hpp"
#include "utilities/number_to_pack_string.hpp"

namespace e_regex::nodes
{
    template<typename matcher,
             unsigned repetitions_min,
             unsigned repetitions_max = std::numeric_limits<unsigned>::max(),
             typename... children>
    struct possessive : public base<matcher, children...>
    {
            // If matcher is optional (aka repetitions_min==0),
            // admission set must include children too
            using admission_set = std::conditional_t<
                repetitions_min == 0,
                typename extract_admission_set<matcher, children...>::type,
                typename decltype(matcher::meta)::admission_set>;

            static constexpr auto meta = e_regex::meta<admission_set> {
                .policy_ = e_regex::policy::POSSESSIVE,
                .minimum_match_size
                = matcher::meta.minimum_match_size * repetitions_min,
                .maximum_match_size
                = repetitions_max == std::numeric_limits<unsigned>::max()
                      ? std::numeric_limits<unsigned>::max()
                      : matcher::meta.maximum_match_size * repetitions_max,
            };

            static constexpr auto expression = []() {
                auto quantifier = []() {
                    if constexpr (repetitions_min == 0
                                  && repetitions_max
                                         == std::numeric_limits<unsigned>::max())
                    {
                        return static_string {"*+"};
                    }
                    else if constexpr (repetitions_min == 1
                                       && repetitions_max
                                              == std::numeric_limits<
                                                  unsigned>::max())
                    {
                        return static_string {"++"};
                    }
                    else if constexpr (repetitions_min == 0
                                       && repetitions_max == 1)
                    {
                        return static_string {"?+"};
                    }
                    else if constexpr (repetitions_max
                                       == std::numeric_limits<unsigned>::max())
                    {
                        return '{'
                               + number_to_pack_string_t<repetitions_min>::string
                               + ",}+";
                    }
                    else
                    {
                        return '{'
                               + number_to_pack_string_t<repetitions_min>::string
                               + ','
                               + number_to_pack_string_t<repetitions_max>::string
                               + "}+";
                    }
                }();

                auto self = matcher::expression;

                if constexpr (sizeof...(children) <= 1)
                {
                    return self + quantifier
                           + get_children_expression<children...>();
                }
                else
                {
                    return self + quantifier + "(?:"
                           + +get_children_expression<children...>()
                           + ')';
                }
            }();

            template<typename... injected_children>
            using optimize
                = possessive<typename matcher::template optimize<>,
                             repetitions_min,
                             repetitions_max,
                             typename children::template optimize<>...>;

            template<typename... injected_children>
            static constexpr auto match(auto& result) -> auto&
            {
                if (result.actual_iterator_end > result.query.end())
                {
                    result = false;
                    return result;
                }

                for (unsigned i = 0; i < repetitions_max; ++i)
                {
                    auto last_result = result;
                    matcher::template match<injected_children...>(
                        last_result);

                    if (last_result)
                    {
                        result = last_result;
                    }
                    else
                    {
                        if (i < repetitions_min)
                        {
                            // Failed too early
                            result.accepted = false;
                            return result;
                        }

                        // Failed but repetitions_min was satisfied,
                        // run dfs
                        break;
                    }
                }

                return dfs<std::tuple<children...>>(result);
            }
    };

} // namespace e_regex::nodes

#endif /* E_REGEX_NODES_POSSESSIVE_HPP_*/
