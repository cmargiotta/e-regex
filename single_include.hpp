#ifndef E_REGEX
#define E_REGEX

#ifndef MATCH_RESULT
#define MATCH_RESULT

#include <array>

#ifndef UTILITIES_LITERAL_STRING_VIEW
#define UTILITIES_LITERAL_STRING_VIEW

#include <functional>
#include <string_view>

namespace e_regex
{
    template<typename Type = char>
    class literal_string_view
    {
        public:
            using iterator = const Type*;

        private:
            iterator begin_;
            iterator end_;

        public:
            constexpr literal_string_view(): begin_ {nullptr}, end_ {nullptr}
            {
            }

            constexpr literal_string_view(iterator begin_, iterator end_)
                : begin_ {begin_}, end_ {end_}
            {
            }

            template<std::size_t size>
            constexpr literal_string_view(const char (&data)[size]) noexcept
                : begin_ {std::begin(data)}, end_ {std::end(data) - 1}
            {
            }

            constexpr literal_string_view(std::string_view data)
                : begin_ {data.begin()}, end_ {data.end()}
            {
            }

            constexpr literal_string_view(const literal_string_view& other) noexcept = default;
            constexpr literal_string_view(literal_string_view&& other) noexcept      = default;
            constexpr auto operator=(const literal_string_view& other) noexcept
                -> literal_string_view& = default;
            constexpr auto operator=(literal_string_view&& other) noexcept
                -> literal_string_view&               = default;
            constexpr ~literal_string_view() noexcept = default;

            constexpr auto begin() noexcept
            {
                return begin_;
            }

            constexpr auto end() noexcept
            {
                return end_;
            }

            constexpr auto begin() const noexcept
            {
                return begin_;
            }

            constexpr auto end() const noexcept
            {
                return end_;
            }

            constexpr auto size() const noexcept
            {
                return end_ - begin_;
            }

            constexpr operator std::string_view() const noexcept
            {
                return std::string_view {begin(), end()};
            }

            constexpr bool operator==(literal_string_view other) const
            {
                if (begin() == other.begin() && end() == other.end())
                {
                    return true;
                }

                if (size() != other.size())
                {
                    return false;
                }

                for (auto i = 0; i < size(); ++i)
                {
                    if (begin()[i] != other.begin()[i])
                    {
                        return false;
                    }
                }

                return true;
            }

            constexpr auto empty() const noexcept
            {
                return size() == 0;
            }
    };
}// namespace e_regex

#endif /* UTILITIES_LITERAL_STRING_VIEW */

namespace e_regex
{
    template<std::size_t groups, typename Char_Type>
    struct match_result_data
    {
            literal_string_view<Char_Type>                     query;
            typename literal_string_view<Char_Type>::iterator  actual_iterator_start;
            typename literal_string_view<Char_Type>::iterator  actual_iterator_end;
            typename literal_string_view<Char_Type>::iterator  last_group_start;
            std::array<literal_string_view<Char_Type>, groups> match_groups;
            std::size_t                                        matches  = 0;
            bool                                               accepted = true;

            constexpr auto operator=(bool accepted) noexcept -> match_result_data&
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return accepted;
            }
    };

    template<typename matcher, typename Char_Type = char>
    class match_result
    {
            friend matcher;

        public:
            match_result_data<matcher::groups, Char_Type> data;
            bool                                          initialized = false;

        private:
            constexpr void init()
            {
                initialized              = true;
                data.matches             = 0;
                data.match_groups        = {};
                data.actual_iterator_end = data.actual_iterator_start;
                data.last_group_start    = data.actual_iterator_start;
                data.accepted            = true;

                data = matcher::match(std::move(data));
            }

        public:
            constexpr match_result(literal_string_view<> query) noexcept
            {
                data.query                 = query;
                data.actual_iterator_start = query.begin();

                this->next();
            }

            constexpr auto operator=(bool accepted) noexcept -> match_result&
            {
                this->accepted = accepted;

                return *this;
            }

            constexpr operator bool() const noexcept
            {
                return data;
            }

            constexpr auto is_accepted() const noexcept
            {
                return operator bool();
            }

            template<std::size_t index>
            constexpr auto get() const noexcept
            {
                static_assert(index <= matcher::groups,
                              "Group index is greater than the number of groups.");

                return get_group(index);
            }

            constexpr auto get_group(std::size_t index) const noexcept
            {
                if (index == 0)
                {
                    return to_view();
                }

                return static_cast<std::string_view>(data.match_groups[index - 1]);
            }

            constexpr auto operator[](std::size_t index) const noexcept
            {
                return get_group(index);
            }

            constexpr auto to_view() const noexcept
            {
                if (!is_accepted())
                {
                    return std::string_view {};
                }

                return std::string_view {data.actual_iterator_start, data.actual_iterator_end};
            }

            constexpr operator literal_string_view<Char_Type>() const noexcept
            {
                return to_view();
            }

            constexpr auto size() const noexcept
            {
                return data.matches;
            }

            static constexpr auto groups() noexcept
            {
                return matcher::groups;
            }

            /**
             * @brief Iterate matches
             *
             * @return false if there are no other matches
             */
            constexpr auto next() noexcept
            {
                do
                {
                    if (initialized)
                    {
                        // This is not the first call
                        if (data.accepted)
                        {
                            data.actual_iterator_start = data.actual_iterator_end;
                        }
                        else
                        {
                            data.actual_iterator_start++;
                        }
                    }

                    init();
                } while (!data.accepted && data.actual_iterator_start < data.query.end()
                         && data.actual_iterator_start >= data.query.begin());

                return data.accepted;
            }
    };
}// namespace e_regex

// For structured decomposition
namespace std
{
    template<typename matcher>
    struct tuple_size<e_regex::match_result<matcher>>
    {
            static const std::size_t value = matcher::groups + 1;
    };

    template<std::size_t N, typename matcher, typename Char_Type>
    struct tuple_element<N, e_regex::match_result<matcher, Char_Type>>
    {
            using type = std::string_view;
    };

    template<std::size_t N, typename matcher>
    auto get(e_regex::match_result<matcher>&& t) noexcept
    {
        return t.template get<N>();
    }

}// namespace std

#endif /* MATCH_RESULT */

#ifndef STATIC_STRING
#define STATIC_STRING

#include <array>
#include <string_view>
#include <utility>

namespace e_regex
{
    template<std::size_t size_, typename Char_Type = char>
    struct static_string
    {
            static constexpr auto  size = size_ - 1;
            std::array<char, size> data;

            constexpr static_string(const Char_Type (&data)[size_]) noexcept
            {
                std::copy(data, data + size, this->data.begin());
            }

            constexpr static_string(const std::array<Char_Type, size>& data) noexcept: data {data}
            {
            }

            constexpr static_string() noexcept = default;

            constexpr operator literal_string_view<Char_Type>() const noexcept
            {
                return to_view();
            }

            constexpr auto to_view() const noexcept
            {
                return literal_string_view<Char_Type> {data.begin(), data.end()};
            }

            [[nodiscard]] constexpr bool empty() const noexcept
            {
                return size == 0;
            }

            template<std::size_t begin, std::size_t end = size>
            constexpr auto substring() const noexcept
            {
                static_string<end - begin + 1> result;

                std::copy(data.begin() + begin, data.begin() + end, result.data.begin());
                return result;
            }
    };

    template<char... data>
    struct pack_string
    {
            static constexpr auto       size    = sizeof...(data);
            static constexpr std::array string  = {data...};
            static constexpr auto       string_ = static_string<size + 1> {string};
    };

    template<typename S1, typename S2>
    struct merge_pack_strings;

    template<char... data1, char... data2>
    struct merge_pack_strings<pack_string<data1...>, pack_string<data2...>>
    {
            using type = pack_string<data1..., data2...>;
    };

    template<typename S1, typename S2>
    using merge_pack_strings_t = typename merge_pack_strings<S1, S2>::type;

    template<typename separator, typename... strings>
    struct concatenate_pack_strings;

    template<typename separator, typename string>
    struct concatenate_pack_strings<separator, string>
    {
            using type = string;
    };

    template<typename separator, typename string, typename... strings>
    struct concatenate_pack_strings<separator, string, strings...>
    {
            using first_part = merge_pack_strings_t<string, separator>;
            using type
                = merge_pack_strings_t<first_part,
                                       typename concatenate_pack_strings<separator, strings...>::type>;
    };

    template<typename separator, typename... strings>
    using concatenate_pack_strings_t = typename concatenate_pack_strings<separator, strings...>::type;

    template<literal_string_view string>
    constexpr auto to_static_string() noexcept
    {
        static_string<string.size() + 1> result;
        std::copy(string.begin(), string.end(), result.data.begin());

        return result;
    }

    template<std::size_t size>
    constexpr auto to_static_string(auto string) noexcept
    {
        static_string<size + 1> result;

        std::copy(string.begin(), string.end(), result.data.begin());

        return result;
    }

    template<auto instance, typename indices = std::make_index_sequence<instance.size>>
    struct build_pack_string;

    template<auto instance, std::size_t... indices>
    struct build_pack_string<instance, std::index_sequence<indices...>>
    {
            using type = pack_string<instance.data[indices]...>;
    };

    template<auto instance>
    using build_pack_string_t = typename build_pack_string<instance>::type;

}// namespace e_regex

#endif /* STATIC_STRING */

#ifndef TOKENIZATION_RESULT
#define TOKENIZATION_RESULT

#include <array>
#include <string>
#include <vector>

namespace e_regex
{
    template<typename Token_Type, typename String_Type = literal_string_view<char>>
    struct token
    {
            Token_Type  type;
            String_Type string;
    };

    template<typename String_Type>
    struct token<void, String_Type>
    {
            String_Type string;
    };

    // If Token_Type is an enum, it MUST contain at least the number of groups of regex of
    // consecutive values starting from 0
    template<auto matcher, auto separator_verifier, typename Token_Type_, typename Char_Type = char>
    class tokenization_result
    {
        private:
            using match_result = decltype(matcher(std::declval<literal_string_view<>>()));

        public:
            using token_type = token<Token_Type_, literal_string_view<Char_Type>>;

            class iterator
            {
                private:
                    match_result res;
                    token_type   current;

                    [[nodiscard]] static constexpr auto build_token(const match_result& res) noexcept
                    {
                        if constexpr (std::is_same_v<Token_Type_, void>)
                        {
                            // void token type, no need to match a group
                            return decltype(current) {res.to_view()};
                        }
                        else
                        {
                            std::size_t index = 0;

                            if constexpr (match_result::groups() != 0)
                            {
                                index = 1;
                                while (res[index].empty())
                                {
                                    ++index;
                                }
                                index--;
                            }

                            return decltype(current) {.type   = static_cast<Token_Type_>(index),
                                                      .string = res.to_view()};
                        }
                    }

                public:
                    constexpr explicit iterator(match_result res)
                        : res {std::move(res)}, current {build_token(res)}
                    {
                    }

                    constexpr iterator(const iterator& other) noexcept               = default;
                    constexpr auto operator=(const iterator& other) -> iterator&     = default;
                    constexpr iterator(iterator&& other) noexcept                    = default;
                    constexpr auto operator=(iterator&& other) noexcept -> iterator& = default;
                    constexpr ~iterator() noexcept                                   = default;

                    constexpr auto operator++() noexcept -> iterator&
                    {
                        auto last = res.to_view();

                        res.next();
                        current = build_token(res);

                        if (current.string.begin() != last.end())
                        {
                            // Non-consecutive tokens, possible syntax error
                            literal_string_view separator {last.end(), current.string.begin()};

                            if (!separator_verifier(separator).is_accepted())
                            {
                                // Separator is not accepted from the given separator matcher,
                                // syntax error found!
                                invalidate();
                            }
                        }

                        return *this;
                    }

                    constexpr auto operator==(const iterator& other) const noexcept -> bool
                    {
                        return current.string.begin() == other.current.string.begin()
                               || (current.string.empty() && other.current.string.empty());
                    }

                    constexpr auto operator!=(const iterator& other) const noexcept -> bool
                    {
                        return !operator==(other);
                    }

                    constexpr auto operator*() const noexcept
                    {
                        if constexpr (std::is_same_v<Token_Type_, void>)
                        {
                            return current.string;
                        }
                        else
                        {
                            return current;
                        }
                    }

                    constexpr operator bool() const noexcept
                    {
                        return !current.string.empty();
                    }

                    constexpr void invalidate() noexcept
                    {
                        current = {};
                    }
            };

        private:
            match_result regex_result;

        public:
            constexpr explicit tokenization_result(literal_string_view<> expression)
                : regex_result {matcher(expression)} {};

            constexpr auto begin() const noexcept -> iterator
            {
                return iterator {regex_result};
            }

            constexpr auto end() const noexcept -> iterator
            {
                iterator res {regex_result};
                res.invalidate();

                return res;
            }

            constexpr auto count() const noexcept
            {
                std::size_t result = 0;
                auto        i      = begin();

                while (i)
                {
                    ++result;
                    ++i;
                }

                return result;
            }
    };

    template<typename Token_Type, auto... data>
    struct token_container
    {
            static constexpr std::array tokens {token<Token_Type> {data.type, data.string.to_view()}...};
    };

    template<auto... data>
    struct token_container<void, data...>
    {
            static constexpr std::array tokens {data.to_view()...};
    };

    template<typename tokens1, typename tokens2>
    struct merge_tokens;

    template<typename Token_Type, auto... data1, auto... data2>
    struct merge_tokens<token_container<Token_Type, data1...>, token_container<Token_Type, data2...>>
    {
            using type = token_container<Token_Type, data1..., data2...>;
    };

    template<typename T>
    concept has_end = requires(T d)
    {
        d.end();
    };

    template<typename Token_Type, static_string string>
    consteval auto build_token(auto token_with_literal) requires(!std::is_same_v<Token_Type, void>)
    {
        return e_regex::token<Token_Type, decltype(string)> {token_with_literal.type, string};
    };

    template<std::same_as<void> Token_Type, static_string string>
    consteval auto build_token(auto /*unused*/)
    {
        return string;
    };

    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
    struct prebuilt_tokenization_result
    {
            static consteval auto get_string(auto token) noexcept
            {
                if constexpr (has_end<decltype(token)>)
                {
                    return token;
                }
                else
                {
                    return token.string;
                }
            }

            static constexpr auto query = string.to_view();
            static constexpr auto res
                = tokenization_result<matcher, separator_matcher, Token_Type> {query};

            static constexpr auto token_data   = *(res.begin());
            static constexpr auto token_string = get_string(token_data);
            static constexpr auto token_end    = token_string.end() - string.data.begin();
            static constexpr auto token_static_string
                = to_static_string<token_string.size()>(token_string);

            static constexpr auto token {build_token<Token_Type, token_static_string>(token_data)};

            using tokens = typename merge_tokens<
                token_container<Token_Type, token>,
                typename prebuilt_tokenization_result<matcher,
                                                      separator_matcher,
                                                      string.template substring<token_end>(),
                                                      Token_Type>::tokens>::type;
    };

    template<auto matcher, auto separator_matcher, static_string string, typename Token_Type>
    requires(string.empty()) struct prebuilt_tokenization_result<matcher, separator_matcher, string, Token_Type>
    {
            using tokens = token_container<Token_Type>;
    };
}// namespace e_regex

#endif /* TOKENIZATION_RESULT */

#ifndef TREE_BUILDER
#define TREE_BUILDER

#ifndef OPERATORS_OPERATORS
#define OPERATORS_OPERATORS

#ifndef OPERATORS_BASIC_NODE_HPP
#define OPERATORS_BASIC_NODE_HPP

#include <algorithm>
#include <tuple>
#include <type_traits>

#ifndef TERMINALS_COMMON_HPP
#define TERMINALS_COMMON_HPP

namespace e_regex::terminals
{
    template<typename terminal>
    struct terminal_common
    {
            static constexpr auto match(auto result)
            {
                if (result.actual_iterator_end >= result.query.end())
                {
                    result = false;
                }
                else
                {
                    return terminal::match_(std::move(result));
                }

                return result;
            }
    };

    template<typename terminal>
    struct negated_terminal
    {
            static constexpr auto match(auto result)
            {
                result = terminal::match_(std::move(result));
                result = !result.accepted;

                return result;
            }
    };

    template<typename... identifiers>
    struct terminal;

    // Avoids instantiating nodes code for consecutive matchers
    template<typename head, typename... tail>
    struct terminal<head, tail...>
    {
            static constexpr auto match(auto result)
            {
                result = terminal<head>::match(std::move(result));

                if (result)
                {
                    return terminal<tail...>::match(std::move(result));
                }

                return result;
            }
    };

    template<typename terminal>
    struct rebuild_expression;

    template<char... chars>
    struct rebuild_expression<terminal<pack_string<chars...>>>
    {
            using string = pack_string<chars...>;
    };

    template<char... chars, typename seq, typename... tail>
    struct rebuild_expression<terminal<pack_string<chars...>, seq, tail...>>
    {
            using string
                = merge_pack_strings_t<pack_string<chars...>,
                                       typename rebuild_expression<terminal<tail...>>::string>;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_COMMON_HPP */

#ifndef UTILITIES_SUM
#define UTILITIES_SUM

namespace e_regex
{
    consteval auto sum()
    {
        return 0;
    }

    consteval auto sum(auto... n)
    {
        return (n + ...);
    }
}// namespace e_regex

#endif /* UTILITIES_SUM */

namespace e_regex
{
    enum class policy
    {
        GREEDY,
        LAZY,
        POSSESSIVE
    };

    template<typename Child, typename... Children>
    constexpr auto dfs(auto match_result) noexcept
    {
        if constexpr (sizeof...(Children) == 0)
        {
            return Child::match(std::move(match_result));
        }
        else
        {
            auto result = Child::match(match_result);

            match_result.matches += Child::groups;
            match_result = dfs<Children...>(std::move(match_result));

            if (!result || (result.actual_iterator_end < match_result.actual_iterator_end))
            {
                return match_result;
            }

            return result;
        }
    }

    template<typename T>
    concept has_groups = requires(T t) { t.groups; };

    template<typename T>
    struct group_getter
    {
            static constexpr auto value = 0;
    };

    template<has_groups T>
    struct group_getter<T>
    {
            static constexpr auto value = T::groups;
    };

    template<typename matcher,
             typename children             = std::tuple<>,
             std::size_t repetitions_min   = 1,
             std::size_t repetitions_max   = 1,
             policy      repetition_policy = policy::GREEDY,
             bool        grouping          = false>
    struct basic_node;

    template<typename matcher, bool grouping_>
    struct set_node_grouping;

    template<typename matcher, typename children, std::size_t min, std::size_t max, policy policy_, bool grouping, bool grouping_>
    struct set_node_grouping<basic_node<matcher, children, min, max, policy_, grouping>, grouping_>
    {
            using type = basic_node<matcher, children, min, max, policy_, grouping_>;
    };

    template<typename matcher, std::size_t min, std::size_t max, policy policy_>
    struct set_node_range;

    template<typename matcher,
             typename children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping,
             std::size_t min,
             std::size_t max,
             policy      policy_>
    struct set_node_range<basic_node<matcher, children, repetitions_min, repetitions_max, repetition_policy, grouping>,
                          min,
                          max,
                          policy_>
    {
            using type = basic_node<matcher, children, min, max, policy_, grouping>;
    };

    template<typename matcher, bool grouping_>
    using set_node_grouping_t = typename set_node_grouping<matcher, grouping_>::type;

    template<typename matcher, std::size_t min, std::size_t max, policy policy_>
    using set_node_range_t = typename set_node_range<matcher, min, max, policy_>::type;

    template<typename matcher, typename children = std::tuple<>, policy policy_ = matcher::backtracking_policy>
    using grouping_node = basic_node<matcher, children, 1, 1, policy_, true>;

    template<typename matcher, typename children = std::tuple<>, policy policy_ = policy::GREEDY>
    using optional_node = basic_node<matcher, children, 0, 1, policy_, false>;

    template<typename node>
    struct get_expression;

    template<typename... terminals>
    struct get_expression<e_regex::terminals::terminal<terminals...>>
    {
            using type =
                typename e_regex::terminals::rebuild_expression<e_regex::terminals::terminal<terminals...>>::string;
    };

    template<typename matcher, std::size_t repetitions_min, std::size_t repetitions_max, policy repetition_policy, bool grouping>
    struct get_expression<
        basic_node<matcher, std::tuple<>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using type = typename get_expression<matcher>::type;
    };

    template<typename... children, std::size_t repetitions_min, std::size_t repetitions_max, policy repetition_policy, bool grouping>
    struct get_expression<
        basic_node<void, std::tuple<children...>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using type
                = concatenate_pack_strings_t<pack_string<'|'>, typename get_expression<children>::type...>;
    };

    template<typename... matchers,
             typename child,
             typename... children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping>
    struct get_expression<
        basic_node<terminals::terminal<matchers...>, std::tuple<child, children...>, repetitions_min, repetitions_max, repetition_policy, grouping>>
    {
            using matcher_string =
                typename terminals::rebuild_expression<terminals::terminal<matchers...>>::string;

            using type = concatenate_pack_strings_t<pack_string<'|'>,
                                                    matcher_string,
                                                    typename get_expression<child>::type,
                                                    typename get_expression<children>::type...>;
    };

    template<typename node>
    using get_expression_t = typename get_expression<node>::type;
}// namespace e_regex

#endif /* OPERATORS_BASIC_NODE_HPP */

#ifndef OPERATORS_BRACES
#define OPERATORS_BRACES

#include <limits>

#ifndef OPERATORS_COMMON
#define OPERATORS_COMMON

#include <tuple>

#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

#include <tuple>

#ifndef HEURISTICS_EXACT_MATCHERS_HPP
#define HEURISTICS_EXACT_MATCHERS_HPP

#include <tuple>

#ifndef HEURISTICS_COMMON_HPP
#define HEURISTICS_COMMON_HPP

#ifndef TERMINALS_EXACT_MATCHER
#define TERMINALS_EXACT_MATCHER

namespace e_regex::terminals
{
    namespace _private
    {
        constexpr auto exact_match(char identifier, auto result)
        {
            result = identifier == *result.actual_iterator_end;
            result.actual_iterator_end++;

            return result;
        }
    }// namespace _private

    template<typename identifier>
    struct exact_matcher;

    template<char identifier>
    struct exact_matcher<pack_string<identifier>>
        : public terminal_common<exact_matcher<pack_string<identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                return _private::exact_match(identifier, std::move(result));
            }
    };

    template<char... identifier>
    struct exact_matcher<pack_string<identifier...>>
        : public terminal_common<exact_matcher<pack_string<identifier...>>>
    {
            static constexpr auto match_(auto result)
            {
                for (auto c: pack_string<identifier...>::string)
                {
                    result = c == *result.actual_iterator_end;
                    result.actual_iterator_end++;

                    if (!result)
                    {
                        break;
                    }
                }

                return result;
            }
    };

    template<typename identifier>
    struct terminal<identifier> : public exact_matcher<identifier>
    {
            static constexpr bool exact = true;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_EXACT_MATCHER */

#ifndef TERMINALS_TERMINAL
#define TERMINALS_TERMINAL

#ifndef TERMINALS_ALARM
#define TERMINALS_ALARM

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'a'>> : public exact_matcher<pack_string<0x07>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ALARM */

#ifndef ANCHORS_ANCHORS
#define ANCHORS_ANCHORS

#ifndef TERMINALS_ANCHORS_END
#define TERMINALS_ANCHORS_END

namespace e_regex::terminals::anchors
{
    struct end
    {
            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end == result.query.end());

                return result;
            }
    };
}// namespace e_regex::terminals::anchors

#endif /* TERMINALS_ANCHORS_END */

#ifndef ANCHORS_START
#define ANCHORS_START

namespace e_regex::terminals::anchors
{
    struct start
    {
            static constexpr auto match(auto result)
            {
                result = (result.actual_iterator_end == result.query.begin());

                return result;
            }
    };
}// namespace e_regex::terminals::anchors

#endif /* ANCHORS_START */

#endif /* ANCHORS_ANCHORS */

#ifndef TERMINALS_ANY
#define TERMINALS_ANY

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'.'>> : public terminal_common<terminal<pack_string<'.'>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end != '\n';
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ANY */

#ifndef TERMINALS_CARRIAGE_RETURN
#define TERMINALS_CARRIAGE_RETURN

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'r'>> : public exact_matcher<pack_string<'\r'>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_CARRIAGE_RETURN */

#ifndef TERMINALS_DIGIT_CHARACTERS
#define TERMINALS_DIGIT_CHARACTERS

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'd'>> : public terminal_common<terminal<pack_string<'\\', 'd'>>>
    {
            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= '0' && *current <= '9');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'D'>> : public negated_terminal<terminal<pack_string<'\\', 'd'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_DIGIT_CHARACTERS */

#ifndef TERMINALS_ESCAPE
#define TERMINALS_ESCAPE

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'e'>> : public exact_matcher<pack_string<0x1B>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ESCAPE */

#ifndef TERMINALS_ESCAPED
#define TERMINALS_ESCAPED

namespace e_regex::terminals
{
    template<char identifier>
    struct terminal<pack_string<'\\', identifier>>
        : public terminal_common<terminal<pack_string<'\\', identifier>>>
    {
            static constexpr auto match_(auto result)
            {
                result = *result.actual_iterator_end == identifier;

                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_ESCAPED */

#ifndef TERMINALS_FORM_FEED
#define TERMINALS_FORM_FEED

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'f'>> : public exact_matcher<pack_string<0x0C>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_FORM_FEED */

#ifndef TERMINALS_NEW_LINE
#define TERMINALS_NEW_LINE

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'n'>> : public exact_matcher<pack_string<'\n'>>
    {
    };

    template<>
    struct terminal<pack_string<'\\', 'N'>> : public negated_terminal<terminal<pack_string<'\\', 'n'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_NEW_LINE */

#ifndef TERMINALS_RANGE
#define TERMINALS_RANGE

namespace e_regex::terminals
{
    template<typename start, typename end>
    struct range_terminal;

    template<char start, char end>
    struct range_terminal<pack_string<start>, pack_string<end>>
        : public terminal_common<range_terminal<pack_string<start>, pack_string<end>>>
    {
            static constexpr auto match(auto result)
            {
                static_assert(end >= start, "Range [a-b] must respect b >= a");

                auto current = result.actual_iterator_end;

                result = *current >= start && *current <= end;
                result.actual_iterator_end++;

                return result;
            }
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_RANGE */

#ifndef TERMINALS_SPACE_CHARACTERS
#define TERMINALS_SPACE_CHARACTERS

#include <algorithm>
#include <array>

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 's'>> : public terminal_common<terminal<pack_string<'\\', 's'>>>
    {
            static constexpr auto match_(auto result)
            {
                constexpr std::array matched {' ', '\t', '\n', '\r', '\f'};

                result = std::find(matched.begin(), matched.end(), *result.actual_iterator_end)
                         != matched.end();

                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'S'>> : public negated_terminal<terminal<pack_string<'\\', 's'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_SPACE_CHARACTERS */

#ifndef TERMINALS_TAB
#define TERMINALS_TAB

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 't'>> : public exact_matcher<pack_string<'\t'>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_TAB */

#ifndef TERMINALS_WORD_CHARACTERS
#define TERMINALS_WORD_CHARACTERS

namespace e_regex::terminals
{
    template<>
    struct terminal<pack_string<'\\', 'w'>> : public terminal_common<terminal<pack_string<'\\', 'w'>>>
    {
            static constexpr auto match_(auto result)
            {
                auto current = result.actual_iterator_end;

                result = (*current >= 'A' && *current <= 'Z') || (*current >= 'a' && *current <= 'z');
                result.actual_iterator_end++;

                return result;
            }
    };

    template<>
    struct terminal<pack_string<'\\', 'W'>> : public negated_terminal<terminal<pack_string<'\\', 'w'>>>
    {
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_WORD_CHARACTERS */

#endif /* TERMINALS_TERMINAL */

namespace e_regex
{
    template<typename node, typename child>
    struct add_child;
}

#endif /* HEURISTICS_COMMON_HPP */

namespace e_regex
{
    /*
        Regexes like "aaaaabc" become a single matcher with the whole string
    */
    template<char... identifiers, char... child_identifiers, typename... children, policy repetition_policy>
        requires terminals::terminal<pack_string<identifiers...>>::exact
                 && terminals::terminal<pack_string<child_identifiers...>>::exact
    struct add_child<
        basic_node<terminals::terminal<pack_string<identifiers...>>, std::tuple<>, 1, 1, repetition_policy, false>,
        basic_node<terminals::terminal<pack_string<child_identifiers...>>, std::tuple<children...>, 1, 1, repetition_policy, false>>
    {
            using type
                = basic_node<terminals::terminal<pack_string<identifiers..., child_identifiers...>>,
                             std::tuple<children...>,
                             1,
                             1,
                             repetition_policy,
                             false>;
    };
}// namespace e_regex

#endif /* HEURISTICS_EXACT_MATCHERS_HPP */

namespace e_regex
{
    template<typename matcher,
             typename... children,
             std::size_t repetitions_min,
             std::size_t repetitions_max,
             policy      repetition_policy,
             bool        grouping,
             typename child>
    struct add_child<basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, repetition_policy, grouping>,
                     child>
    {
            using type
                = basic_node<matcher, std::tuple<children..., child>, repetitions_min, repetitions_max, repetition_policy, grouping>;
    };

    template<typename child>
    struct add_child<void, child>
    {
            using type = child;
    };

    template<typename node, typename child>
    using add_child_t = typename add_child<node, child>::type;
}// namespace e_regex

#endif /* HEURISTICS_HPP */

#ifndef TOKENIZER
#define TOKENIZER

#include <tuple>

#ifndef UTILITIES_TUPLE_CAT
#define UTILITIES_TUPLE_CAT

#include <tuple>

namespace e_regex
{
    template<typename T1, typename T2>
    struct tuple_cat;

    template<typename... T1, typename... T2>
    struct tuple_cat<std::tuple<T1...>, std::tuple<T2...>>
    {
            using type = std::tuple<T1..., T2...>;
    };

    template<typename... T1, typename T2>
    struct tuple_cat<std::tuple<T1...>, T2>
    {
            using type = std::tuple<T1..., T2>;
    };

    template<typename T1, typename T2>
    using tuple_cat_t = typename tuple_cat<T1, T2>::type;
}// namespace e_regex

#endif /* UTILITIES_TUPLE_CAT */

namespace e_regex
{
    template<typename matches, char... string>
    struct extract_braces_numbers;

    template<char... current, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, ',', tail...>
        : public extract_braces_numbers<
              tuple_cat_t<std::tuple<pack_string<current...>>, std::tuple<pack_string<','>>>,
              tail...>
    {
            // ',' found, first number done
    };

    template<char... current, char head, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, head, tail...>
        : public extract_braces_numbers<std::tuple<pack_string<current..., head>>, tail...>
    {
            // Populating first number
    };

    // Probably gcc bug -> ambiguous template instantiation if compact
    template<typename first, typename second, char... current, char... tail>
    struct extract_braces_numbers<std::tuple<first, second, pack_string<current...>>, '}', tail...>
    {
            // '}' found, finishing
            using numbers
                = std::tuple<pack_string<'{'>, first, second, pack_string<current...>, pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<typename first, typename second, char... tail>
    struct extract_braces_numbers<std::tuple<first, second>, '}', tail...>
    {
            // '}' found, finishing
            using numbers   = std::tuple<pack_string<'{'>, first, second, pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<char... current, char... tail>
    struct extract_braces_numbers<std::tuple<pack_string<current...>>, '}', tail...>
    {
            // '}' found, finishing
            using numbers = std::tuple<pack_string<'{'>, pack_string<current...>, pack_string<'}'>>;
            using remaining = pack_string<tail...>;
    };

    template<typename first, typename second, char head, char... tail>
    struct extract_braces_numbers<std::tuple<first, second>, head, tail...>
        : public extract_braces_numbers<std::tuple<first, second, pack_string<head>>, tail...>
    {
            // Populating second number
    };

    template<char... current, typename first, typename second, char head, char... tail>
    struct extract_braces_numbers<std::tuple<first, second, pack_string<current...>>, head, tail...>
        : public extract_braces_numbers<std::tuple<first, second, pack_string<current..., head>>, tail...>
    {
            // Populating second number
    };

    template<char... string>
    using extract_braces_numbers_t = extract_braces_numbers<std::tuple<pack_string<>>, string...>;

    template<typename current, typename string>
    struct tokenizer;

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<head, tail...>>
    {
            // Simple iteration
            using current_ = tuple_cat_t<current, std::tuple<pack_string<head>>>;
            using tokens   = typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char head, char... tail>
    struct tokenizer<current, pack_string<'\\', head, tail...>>
    {
            // Escaped character
            using current_ = tuple_cat_t<current, std::tuple<pack_string<'\\', head>>>;
            using tokens   = typename tokenizer<current_, pack_string<tail...>>::tokens;
    };

    template<typename current, char... tail>
    struct tokenizer<current, pack_string<'{', tail...>>
    {
            // Number inside braces
            using numbers  = extract_braces_numbers_t<tail...>;
            using current_ = tuple_cat_t<current, typename numbers::numbers>;
            using tokens   = typename tokenizer<current_, typename numbers::remaining>::tokens;
    };

    template<typename current>
    struct tokenizer<current, pack_string<>>
    {
            // Base case
            using tokens = current;
    };

    template<typename string>
    using tokenizer_t = typename tokenizer<std::tuple<>, string>::tokens;
}// namespace e_regex

#endif /* TOKENIZER */

namespace e_regex
{
    template<typename last_node, typename tokens>
    struct tree_builder_helper;

    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<>>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node =
                typename tree_builder_helper<basic_node<terminals::terminal<head>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename matcher>
    struct negated_node
    {
            static constexpr std::size_t groups = matcher::groups;

            static constexpr auto match(auto result)
            {
                result = matcher::match(std::move(result));

                result = !static_cast<bool>(result);

                return result;
            }
    };
}// namespace e_regex

#endif /* OPERATORS_COMMON */

#ifndef SRC_OPERATORS_GREEDY_NODE_HPP
#define SRC_OPERATORS_GREEDY_NODE_HPP

#include <cstddef>

namespace e_regex
{
    namespace _private
    {
        constexpr auto self_match(auto result,
                                  decltype(result) (*match)(decltype(result)),
                                  std::size_t repetitions_max,
                                  std::size_t repetitions_min,
                                  bool        grouping)
        {
            unsigned matches = 0;

            auto last_res = result;

            while (last_res.actual_iterator_end < result.query.end() && matches < repetitions_max)
            {
                last_res.matches = result.matches;// Only last group is considered
                auto res         = match(last_res);

                if (res)
                {
                    if (grouping)
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

            if (result.actual_iterator_end == last_res.actual_iterator_end && repetitions_min != 0)
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

        constexpr auto match(auto result,
                             decltype(result) (*self_match)(decltype(result)),
                             decltype(result) (*dfs)(decltype(result)),
                             std::size_t repetitions_min,
                             bool        grouping,
                             std::size_t children_number,
                             std::size_t groups)
        {
            auto match_index = result.matches;
            auto end         = result.last_group_start;
            auto begin       = end;

            if (grouping)
            {
                result.matches++;
                result.last_group_start = result.actual_iterator_end;
            }

            if (children_number == 0)
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
                    result = dfs(std::move(result));
                }

                // Greedy backtracking
                while (!result && this_start > result.last_group_start + repetitions_min)
                {
                    result = true;
                    this_start--;
                    result.actual_iterator_end = this_start;
                    end                        = this_start;

                    result = dfs(std::move(result));

                    if (result)
                    {
                        break;
                    }
                }
            }

            if (grouping)
            {
                if (result)
                {
                    result.match_groups[match_index] = literal_string_view {begin, end};
                    result.matches += groups;
                }
            }

            if (repetitions_min == 0)
            {
                result = true;
            }

            return result;
        }
    }// namespace _private

    template<typename matcher, typename... children, std::size_t repetitions_min, std::size_t repetitions_max, bool grouping>
    struct basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, policy::GREEDY, grouping>
    {
            static constexpr auto backtracking_policy = policy::GREEDY;

            static constexpr std::size_t groups
                = group_getter<matcher>::value + sum(children::groups...) + (grouping ? 1 : 0);

            template<typename Res>
            static constexpr Res self_match(Res result)
            {
                if constexpr (std::is_same_v<matcher, void>)
                {
                    return result;
                }
                else
                {
                    return _private::self_match(result,
                                                &matcher::template match<decltype(result)>,
                                                repetitions_max,
                                                repetitions_min,
                                                grouping);
                }
            }

            template<typename Res>
            static constexpr Res match(Res result)
            {
                decltype(result) (*dfs_ptr)(decltype(result)) = nullptr;
                if constexpr (sizeof...(children) != 0)
                {
                    dfs_ptr = &dfs<children...>;
                }

                return _private::match(
                    result, &self_match<Res>, dfs_ptr, repetitions_min, grouping, sizeof...(children), groups);
            }
    };
}// namespace e_regex

#endif /* SRC_OPERATORS_GREEDY_NODE_HPP */

#ifndef UTILITIES_EXTRACT_DELIMITED_CONTENT
#define UTILITIES_EXTRACT_DELIMITED_CONTENT

#include <tuple>

namespace e_regex
{
    template<char open, char closing, unsigned skip_counter, typename current, typename string>
    struct extract_delimited_content;

    template<char open, char closing, typename current, typename... tail_>
    struct extract_delimited_content<open, closing, 0, current, std::tuple<pack_string<closing>, tail_...>>
    {
            // Base case, closing delimiter found
            using result    = current;
            using remaining = std::tuple<tail_...>;
    };

    template<char open, char closing, typename current>
    struct extract_delimited_content<open, closing, 0, current, std::tuple<>>
    {
            // Base case, malformed token string
            using result    = std::tuple<>;
            using remaining = result;
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<pack_string<closing>, tail...>>
        : public extract_delimited_content<open,
                                           closing,
                                           skip_counter - 1,
                                           tuple_cat_t<current, pack_string<closing>>,
                                           std::tuple<tail...>>
    {
            // Closing delimiter found, but it has to be skipped
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<pack_string<open>, tail...>>
        : public extract_delimited_content<open,
                                           closing,
                                           skip_counter + 1,
                                           tuple_cat_t<current, pack_string<open>>,
                                           std::tuple<tail...>>
    {
            // Open delimiter found, increase skip counter
    };

    template<char open, char closing, unsigned skip_counter, typename current, typename head, typename... tail>
    struct extract_delimited_content<open, closing, skip_counter, current, std::tuple<head, tail...>>
        : public extract_delimited_content<open, closing, skip_counter, tuple_cat_t<current, head>, std::tuple<tail...>>
    {
            // Iterate characters
    };

    template<char open, char closing, typename string>
    using extract_delimited_content_t
        = extract_delimited_content<open, closing, 0, std::tuple<>, string>;
}// namespace e_regex

#endif /* UTILITIES_EXTRACT_DELIMITED_CONTENT */

#ifndef UTILITIES_PACK_STRING_TO_NUMBER
#define UTILITIES_PACK_STRING_TO_NUMBER

namespace e_regex
{
    template<typename string, std::size_t index = string::size - 1>
    struct pack_string_to_number;

    template<char head, char... tail, std::size_t index>
    struct pack_string_to_number<pack_string<head, tail...>, index>
    {
            static consteval auto ten_power(std::size_t exp) -> std::size_t
            {
                std::size_t res = 1;

                while (exp-- != 0)
                {
                    res *= 10;
                }

                return res;
            }

            static inline const constinit std::size_t value
                = ((head - '0') * ten_power(index))
                  + pack_string_to_number<pack_string<tail...>, index - 1>::value;
    };

    template<std::size_t index>
    struct pack_string_to_number<pack_string<>, index>
    {
            static constexpr std::size_t value = 0;
    };
}// namespace e_regex

#endif /* UTILITIES_PACK_STRING_TO_NUMBER */

namespace e_regex
{
    template<typename data>
    struct first_type;

    template<typename head, typename... tail>
    struct first_type<std::tuple<head, tail...>>
    {
            using type      = head;
            using remaining = std::tuple<tail...>;
    };

    template<>
    struct first_type<std::tuple<>>
    {
            using type      = void;
            using remaining = std::tuple<>;
    };

    template<typename data>
    using first_type_t = typename first_type<data>::type;

    template<typename matcher, typename data, policy policy_ = policy::GREEDY>
    struct quantified_node_builder;

    template<typename matcher, typename first, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first>, policy_>
    {
            static constexpr auto value = pack_string_to_number<first>::value;
            using type                  = set_node_range_t<matcher, value, value, policy_>;
    };

    template<typename matcher, typename first, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, policy_>
    {
            using type = set_node_range_t<matcher,
                                          pack_string_to_number<first>::value,
                                          std::numeric_limits<std::size_t>::max(),
                                          policy_>;
    };

    template<typename matcher, typename first, typename second, policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, policy_>
    {
            using type
                = set_node_range_t<matcher, pack_string_to_number<first>::value, pack_string_to_number<second>::value, policy_>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using remaining_head = first_type<typename substring::remaining>;

            static constexpr auto policy_
                = std::is_same_v<typename remaining_head::type, pack_string<'?'>>
                      ? policy::LAZY
                      : (std::is_same_v<typename remaining_head::type, pack_string<'+'>>
                             ? policy::POSSESSIVE
                             : policy::GREEDY);

            using remaining = std::conditional_t<policy_ != policy::GREEDY,
                                                 typename remaining_head::remaining,
                                                 typename substring::remaining>;

            using new_node =
                typename quantified_node_builder<last_node, typename substring::result, policy_>::type;

            using tree = typename tree_builder_helper<new_node, remaining>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES */

#ifndef OPERATORS_END_ANCHOR
#define OPERATORS_END_ANCHOR

namespace e_regex
{
    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'$'>>>
    {
            // End anchor found

            using new_node
                = basic_node<terminals::anchors::end, std::tuple<>, 1, 1, policy::POSSESSIVE>;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_END_ANCHOR */

#ifndef OPERATORS_HEX
#define OPERATORS_HEX

namespace e_regex
{
    template<char C>
    concept hex = C >= '0' && C <= 'F';

    template<char... digits>
    struct hex_to_bin;

    template<char first, char second, char... tail>
    struct hex_to_bin<first, second, tail...>
    {
            static_assert(hex<first> && hex<second>, "Hex characters must be included between 0 and F");

            static constexpr char current = ((first - '0') << 4) | (second - '0');

            using result
                = merge_pack_strings_t<pack_string<current>, typename hex_to_bin<tail...>::result>;
    };

    template<char first, char second, char third>
    struct hex_to_bin<first, second, third>
    {
            static_assert(hex<first> && hex<second> && hex<third>,
                          "Hex characters must be included between 0 and F");

            static constexpr char current        = ((first - '0') << (4)) | (second - '0');
            static constexpr char current_second = (third - '0') << 4;

            using result = pack_string<current>;
    };

    template<char first>
    struct hex_to_bin<first>
    {
            static_assert(hex<first>, "Hex characters must be included between 0 and F");

            static constexpr char current = first - '0';

            using result = pack_string<current>;
    };

    template<>
    struct hex_to_bin<>
    {
            using result = pack_string<>;
    };

    template<typename digits>
    struct hex_tuple_to_bin;

    template<char... digits>
    struct hex_tuple_to_bin<std::tuple<pack_string<digits...>>>
    {
            using result = typename hex_to_bin<digits...>::result;
    };

    template<typename last_node, char first_nibble, char second_nibble, typename... tail>
    requires hex<first_nibble> && hex<second_nibble>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', 'x'>, pack_string<first_nibble>, pack_string<second_nibble>, tail...>>
    {
            using value = typename hex_to_bin<first_nibble, second_nibble>::result;

            using new_node =
                typename tree_builder_helper<basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, char nibble, typename... tail>
    requires hex<nibble>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<nibble>, tail...>>
    {
            using value = typename hex_to_bin<nibble>::result;

            using new_node =
                typename tree_builder_helper<basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename hex_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_HEX */

#ifndef OPERATORS_LAZY_NODE_HPP
#define OPERATORS_LAZY_NODE_HPP

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

#endif /* OPERATORS_LAZY_NODE_HPP */

#ifndef OPERATORS_OCTAL
#define OPERATORS_OCTAL

namespace e_regex
{
    template<char C>
    concept octal = C >= '0' && C <= '8';

    template<char... digits>
    struct octal_to_bin;

    template<char first, char second, char third, char... tail>
    struct octal_to_bin<first, second, third, tail...>
    {
            static_assert(octal<first> && octal<second> && octal<third>,
                          "Octal characters must be included between 0 and 8");

            static constexpr char current
                = ((first - '0') << 6) | ((second - '0') << 3) | (third - '0');

            using result
                = merge_pack_strings_t<pack_string<current>, typename octal_to_bin<tail...>::result>;
    };

    template<>
    struct octal_to_bin<>
    {
            using result = pack_string<>;
    };

    template<typename digits>
    struct octal_tuple_to_bin;

    template<char... digits>
    struct octal_tuple_to_bin<std::tuple<pack_string<digits...>>>
    {
            using result = typename octal_to_bin<digits...>::result;
    };

    template<typename last_node, char first_nibble, char second_nibble, char third_nibble, typename... tail>
    requires octal<first_nibble> && octal<second_nibble> && octal<third_nibble>
    struct tree_builder_helper<
        last_node,
        std::tuple<pack_string<'\\', first_nibble>, pack_string<second_nibble>, pack_string<third_nibble>, tail...>>
    {
            using value = typename octal_to_bin<first_nibble, second_nibble, third_nibble>::result;

            using new_node =
                typename tree_builder_helper<basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'o'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename octal_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_OCTAL */

#ifndef OPERATORS_OPTIONAL
#define OPERATORS_OPTIONAL

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, tail...>>
    {
            // greedy ? operator found
            using new_node = set_node_range_t<last_node, 0, 1, policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'?'>, tail...>>
    {
            // lazy ? operator found
            using new_node = set_node_range_t<last_node, 0, 1, policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'+'>, tail...>>
    {
            // possessive ? operator found
            using new_node = set_node_range_t<last_node, 0, 1, policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_OPTIONAL */

#ifndef OPERATORS_PLUS
#define OPERATORS_PLUS

#include <limits>

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, tail...>>
    {
            // greedy + operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, pack_string<'?'>, tail...>>
    {
            // lazy + operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, pack_string<'+'>, tail...>>
    {
            // lazy + operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_PLUS */

#ifndef OPERATORS_POSSESSIVE_NODE_HPP
#define OPERATORS_POSSESSIVE_NODE_HPP

namespace e_regex
{
    template<typename matcher, typename... children, std::size_t repetitions_min, std::size_t repetitions_max, bool grouping>
    struct basic_node<matcher, std::tuple<children...>, repetitions_min, repetitions_max, policy::POSSESSIVE, grouping>
    {
            static constexpr auto backtracking_policy = policy::POSSESSIVE;

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

                    while (last_res.actual_iterator_end <= result.query.end()
                           && matches < repetitions_max)
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

                    result = std::move(last_res);
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

                    if (result)
                    {
                        begin  = result.last_group_start;
                        end    = result.actual_iterator_end;
                        result = dfs<children...>(std::move(result));
                    }
                }

                if constexpr (grouping)
                {
                    if (result)
                    {
                        result.match_groups[match_index] = literal_string_view {begin, end};
                    }
                }

                return result;
            }
    };
}// namespace e_regex

#endif /* OPERATORS_POSSESSIVE_NODE_HPP */

#ifndef OPERATORS_ROUND_BRACKETS
#define OPERATORS_ROUND_BRACKETS

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'('>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<grouping_node<subregex>, typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node,
                               std::tuple<pack_string<'('>, pack_string<'?'>, pack_string<':'>, tail...>>
    {
            // Non capturing group found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<basic_node<subregex, std::tuple<>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_ROUND_BRACKETS */

#ifndef OPERATORS_SQUARE_BRACKETS
#define OPERATORS_SQUARE_BRACKETS

namespace e_regex
{
    template<typename last_node, typename tokens>
    struct square_bracker_tree_builder_helper;

    template<typename last_node>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<>>
    {
            // Base case

            using tree = last_node;
    };

    template<typename last_node, typename head, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<head, tail...>>
    {
            // Simple case, iterate

            using new_node
                = add_child_t<last_node, basic_node<terminals::exact_matcher<head>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, char identifier, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<pack_string<'\\', identifier>, tail...>>
    {
            // Simple case, iterate

            using new_node
                = add_child_t<last_node,
                              basic_node<terminals::terminal<pack_string<'\\', identifier>>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename start, typename end, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<start, pack_string<'-'>, end, tail...>>
    {
            // Range found

            using new_node
                = add_child_t<last_node, basic_node<terminals::range_terminal<start, end>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, tail...>>
    {
            // [ found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<basic_node<void, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<subregex, typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, pack_string<'^'>, tail...>>
    {
            // [ found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<basic_node<void, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<basic_node<negated_node<subregex>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

}// namespace e_regex

#endif /* OPERATORS_SQUARE_BRACKETS */

#ifndef OPERATORS_STAR
#define OPERATORS_STAR

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, tail...>>
    {
            // greedy * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'?'>, tail...>>
    {
            // lazy * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'+'>, tail...>>
    {
            // possessive * operator found
            using new_node
                = basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_STAR */

#ifndef OPERATORS_START_ANCHOR
#define OPERATORS_START_ANCHOR

namespace e_regex
{
    template<typename... tail>
    struct tree_builder_helper<void, std::tuple<pack_string<'^'>, tail...>>
    {
            // Start anchor found

            using tree = typename tree_builder_helper<
                basic_node<terminals::anchors::start, std::tuple<>, 1, 1, policy::POSSESSIVE>,
                std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_START_ANCHOR */

#endif /* OPERATORS_OPERATORS */

#ifndef UTILITIES_SPLIT
#define UTILITIES_SPLIT

#include <tuple>

#ifndef UTILITIES_REVERSE
#define UTILITIES_REVERSE

#include <tuple>

namespace e_regex
{
    template<typename T, typename current = std::tuple<>>
    struct reverse;

    template<typename T, typename... tail, typename... currents>
    struct reverse<std::tuple<T, tail...>, std::tuple<currents...>>
    {
            using type = typename reverse<std::tuple<tail...>, std::tuple<T, currents...>>::type;
    };

    template<typename... currents>
    struct reverse<std::tuple<>, std::tuple<currents...>>
    {
            using type = std::tuple<currents...>;
    };

    template<typename tuple>
    using reverse_t = typename reverse<tuple>::type;
}// namespace e_regex

#endif /* UTILITIES_REVERSE */

namespace e_regex
{
    template<char separator, typename tokens, typename current = std::tuple<std::tuple<>>>
    struct split;

    template<char separator, typename... tail, typename... current_tokens, typename... currents>
    struct split<separator,
                 std::tuple<pack_string<separator>, tail...>,
                 std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            using current = std::tuple<std::tuple<>, std::tuple<current_tokens...>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename head, typename... tail, typename... current_tokens, typename... currents>
    struct split<separator, std::tuple<head, tail...>, std::tuple<std::tuple<current_tokens...>, currents...>>
    {
            using current = std::tuple<std::tuple<current_tokens..., head>, currents...>;
            using type    = typename split<separator, std::tuple<tail...>, current>::type;
    };

    template<char separator, typename current>
    struct split<separator, std::tuple<>, current>
    {
            using type = reverse_t<current>;
    };

    template<char separator, typename tokens>
    using split_t = typename split<separator, tokens>::type;
}// namespace e_regex

#endif /* UTILITIES_SPLIT */

namespace e_regex
{
    template<typename tokens>
    struct tree_builder_branches;

    template<typename... subregexes>
    struct tree_builder_branches<std::tuple<subregexes...>>
    {
            using branches = std::tuple<typename tree_builder_helper<void, subregexes>::tree...>;
            using tree     = basic_node<void, branches>;
    };

    template<typename regex>
    struct tree_builder;

    template<char... regex>
    struct tree_builder<pack_string<regex...>>
        : public tree_builder_branches<split_t<'|', tokenizer_t<pack_string<regex...>>>>
    {
    };

}// namespace e_regex

#endif /* TREE_BUILDER */

namespace e_regex
{
    template<static_string regex>
    constexpr auto match = [](literal_string_view<> expression)
    {
        using packed  = build_pack_string_t<regex>;
        using matcher = typename tree_builder<packed>::tree;

        return match_result<matcher> {expression};
    };

    template<static_string regex, static_string separator = static_string {""}, typename token_type = void>
    constexpr auto tokenize = [](literal_string_view<> expression)
    {
        auto matcher           = match<regex>;
        auto separator_matcher = match<separator>;

        return tokenization_result<matcher, separator_matcher, token_type> {expression};
    };

    template<static_string regex, static_string data, static_string separator = static_string {""}, typename token_type = void>
    using token_t = prebuilt_tokenization_result<match<regex>, match<separator>, data, token_type>;
}// namespace e_regex

#endif /* E_REGEX */
