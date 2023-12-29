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
#include <utility>

namespace e_regex
{
    template<char... data>
    struct pack_string
    {
            static constexpr auto       size   = sizeof...(data);
            static constexpr std::array string = {data...};
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

    template<std::size_t size_, typename Char_Type = char>
    struct static_string
    {
            static constexpr auto  size = size_ - 1;
            std::array<char, size> data;

            constexpr static_string(const char (&data)[size_]) noexcept
            {
                std::copy(data, data + size, this->data.begin());
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

#ifndef OPERATORS_BASIC_NODE
#define OPERATORS_BASIC_NODE

#include <algorithm>
#include <tuple>
#include <type_traits>

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
    concept has_groups = requires(T t)
    {
        t.groups;
    };

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
}// namespace e_regex

#endif /* OPERATORS_BASIC_NODE */

#ifndef OPERATORS_BRACES
#define OPERATORS_BRACES

#include <limits>

#ifndef OPERATORS_COMMON
#define OPERATORS_COMMON

#include <tuple>

