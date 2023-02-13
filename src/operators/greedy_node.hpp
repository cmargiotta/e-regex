#ifndef OPERATORS_GREEDY_NODE
#define OPERATORS_GREEDY_NODE

#include "basic_node.hpp"

namespace e_regex
{
    template<typename matcher, typename... children, std::size_t repetitions_min, std::size_t repetitions_max, bool grouping>
    struct basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, policy::GREEDY, grouping>
    {
            static constexpr auto backtracking_policy = policy::GREEDY;

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

                    while (last_res.actual_iterator_end < result.query.end() && matches < repetitions_max)
                    {
                        last_res.matches = result.matches;// Only last group is considered
                        auto res         = matcher::match(last_res);

                        if (res)
                        {
                            if constexpr (grouping)
                            {
                                res.last_group_start = last_res.actual_iterator_end;
                            }

                            last_res = std::move(res);
                            matches++;
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (result.actual_iterator_end == last_res.actual_iterator_end
                        && repetitions_min != 0)
                    {
                        // No matching characters found and this node is not optional, increase
                        // iterator
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
                auto begin       = end;

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
                    result.last_group_start = result.actual_iterator_end;
                    result                  = self_match(std::move(result));
                    auto this_start         = result.actual_iterator_end;

                    if (result)
                    {
                        begin  = result.last_group_start;
                        end    = result.actual_iterator_end;
                        result = dfs<children...>(std::move(result));
                    }

                    // Greedy backtracking
                    while (!result && this_start > result.last_group_start + repetitions_min)
                    {
                        result = true;
                        this_start--;
                        result.actual_iterator_end = this_start;
                        end                        = this_start;

                        result = dfs<children...>(std::move(result));

                        if (result)
                        {
                            break;
                        }
                    }
                }

                if constexpr (grouping)
                {
                    if (result)
                    {
                        result.match_groups[match_index] = literal_string_view {begin, end};
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

#endif /* OPERATORS_GREEDY_NODE */
