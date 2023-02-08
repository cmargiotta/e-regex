#ifndef OPERATORS_LAZY_NODE_HPP
#define OPERATORS_LAZY_NODE_HPP

#include "basic_node.hpp"

namespace e_regex
{
    template<typename matcher, typename... children, std::size_t repetitions_min, std::size_t repetitions_max, bool grouping>
    struct basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, policy::LAZY, grouping>
    {
            static constexpr auto backtracking_policy = policy::LAZY;

            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(children::groups...) + (grouping ? 1 : 0);

            static constexpr auto self_match(auto result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return result;
                }
                else
                {
                    unsigned matches = 0;

                    auto last_res = result;

                    auto res = matcher::match(last_res);

                    if (res)
                    {
                        if constexpr (grouping)
                        {
                            res.last_group_start = last_res.actual_iterator_end;
                        }

                        last_res = std::move(res);
                        matches++;
                    }

                    if (result.actual_iterator_end == last_res.actual_iterator_end
                        && repetitions_min != 0)
                    {
                        result.actual_iterator_end++;
                    }
                    else
                    {
                        result = std::move(last_res);
                    }

                    result = matches >= repetitions_min && matches <= repetitions_max;

                    return result;
                }
            }

            static constexpr auto match(auto result)
            {
                auto match_index = result.matches;
                auto end         = result.last_group_start;
                auto begin       = result.actual_iterator_end;

                if constexpr (grouping)
                {
                    result.matches++;
                    result.last_group_start = result.actual_iterator_end;
                }

                if constexpr (sizeof...(children) == 0)
                {
                    // No children, no need to apply the policy
                    result = self_match(std::move(result));
                    begin  = result.last_group_start;
                    end    = result.actual_iterator_end;
                }
                else
                {
                    std::size_t total_matches = 0;

                    if constexpr (repetitions_min == 0)
                    {
                        // First try, only dfs
                        result = dfs<children...>(std::move(result));
                        end    = begin;
                    }

                    if (!result || repetitions_min > 0)
                    {
                        // Lazy backtracking
                        do
                        {
                            result = true;

                            do
                            {
                                result = self_match(std::move(result));
                                total_matches++;
                            } while (total_matches < repetitions_min);

                            if (!result)
                            {
                                break;
                            }

                            begin  = result.last_group_start;
                            end    = result.actual_iterator_end;
                            result = dfs<children...>(std::move(result));

                            if (!result)
                            {
                                result.actual_iterator_end++;
                            }

                        } while (!result && result.actual_iterator_end <= result.query.end()
                                 && total_matches < repetitions_max);
                    }
                }

                if constexpr (grouping)
                {
                    if (result)
                    {
                        result.match_groups[match_index] = std::string_view {begin, end};
                    }
                }

                if constexpr (repetitions_min == 0)
                {
                    result = true;
                }

                return result;
            }
    };
}// namespace e_regex

#endif /* OPERATORS_LAZY_NODE_HPP */
