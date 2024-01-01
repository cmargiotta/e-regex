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

            constexpr auto operator=(bool accepted) noexcept -> match_result_data &
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

            constexpr auto operator=(bool accepted) noexcept -> match_result &
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
    auto get(e_regex::match_result<matcher> t) noexcept
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
            static constexpr auto size   = sizeof...(data);
            static constexpr auto string = static_string<size + 1> {std::array {data...}};
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

#ifndef TREE_BUILDER_HPP
#define TREE_BUILDER_HPP

#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#ifndef OPERATORS_BRACES_HPP
#define OPERATORS_BRACES_HPP

#include <limits>

#include <nodes.hpp>

#ifndef OPERATORS_COMMON
#define OPERATORS_COMMON

#include <tuple>

#include <heuristics.hpp>

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
                typename tree_builder_helper<nodes::basic_node<terminals::terminal<head>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_COMMON */

#ifndef UTILITIES_EXTRACT_DELIMITED_CONTENT
#define UTILITIES_EXTRACT_DELIMITED_CONTENT

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

    template<typename matcher, typename data, nodes::policy policy_ = nodes::policy::GREEDY>
    struct quantified_node_builder;

    template<typename matcher, typename first, nodes::policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first>, policy_>
    {
            static constexpr auto value = pack_string_to_number<first>::value;
            using type                  = nodes::set_node_range_t<matcher, value, value, policy_>;
    };

    template<typename matcher, typename first, nodes::policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>>, policy_>
    {
            using type = nodes::set_node_range_t<matcher,
                                                 pack_string_to_number<first>::value,
                                                 std::numeric_limits<std::size_t>::max(),
                                                 policy_>;
    };

    template<typename matcher, typename first, typename second, nodes::policy policy_>
    struct quantified_node_builder<matcher, std::tuple<first, pack_string<','>, second>, policy_>
    {
            using type = nodes::set_node_range_t<matcher,
                                                 pack_string_to_number<first>::value,
                                                 pack_string_to_number<second>::value,
                                                 policy_>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'{'>, tail...>>
    {
            // { found
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using remaining_head = first_type<typename substring::remaining>;

            static constexpr auto policy_
                = std::is_same_v<typename remaining_head::type, pack_string<'?'>>
                      ? nodes::policy::LAZY
                      : (std::is_same_v<typename remaining_head::type, pack_string<'+'>>
                             ? nodes::policy::POSSESSIVE
                             : nodes::policy::GREEDY);

            using remaining = std::conditional_t<policy_ != nodes::policy::GREEDY,
                                                 typename remaining_head::remaining,
                                                 typename substring::remaining>;

            using new_node =
                typename quantified_node_builder<last_node, typename substring::result, policy_>::type;

            using tree = typename tree_builder_helper<new_node, remaining>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_BRACES_HPP */

#ifndef OPERATORS_END_ANCHOR_HPP
#define OPERATORS_END_ANCHOR_HPP

#include <tuple>

#include <heuristics.hpp>
#include <nodes.hpp>
#include <static_string.hpp>
#include <terminals/anchors/end.hpp>

namespace e_regex
{
    template<typename last_node>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'$'>>>
    {
            // End anchor found

            using new_node
                = nodes::basic_node<terminals::anchors::end, std::tuple<>, 1, 1, nodes::policy::POSSESSIVE>;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_END_ANCHOR_HPP */

#ifndef OPERATORS_HEX
#define OPERATORS_HEX

#include <static_string.hpp>
#include <terminals.hpp>
#include <utilities/extract_delimited_content.hpp>

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
                typename tree_builder_helper<nodes::basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, char nibble, typename... tail>
        requires hex<nibble>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<nibble>, tail...>>
    {
            using value = typename hex_to_bin<nibble>::result;

            using new_node =
                typename tree_builder_helper<nodes::basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'x'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename hex_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<nodes::basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_HEX */

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
                typename tree_builder_helper<nodes::basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             std::tuple<tail...>>::tree;
            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'\\', 'o'>, pack_string<'{'>, tail...>>
    {
            using substring = extract_delimited_content_t<'{', '}', std::tuple<tail...>>;

            using value = typename octal_tuple_to_bin<typename substring::result>::result;

            using new_node =
                typename tree_builder_helper<nodes::basic_node<terminals::exact_matcher<value>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_OCTAL */

#ifndef OPERATORS_OPTIONAL
#define OPERATORS_OPTIONAL

#include <nodes.hpp>

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, tail...>>
    {
            // greedy ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'?'>, tail...>>
    {
            // lazy ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'?'>, pack_string<'+'>, tail...>>
    {
            // possessive ? operator found
            using new_node = nodes::set_node_range_t<last_node, 0, 1, nodes::policy::POSSESSIVE>;

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
                = nodes::basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), nodes::policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, pack_string<'?'>, tail...>>
    {
            // lazy + operator found
            using new_node
                = nodes::basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), nodes::policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'+'>, pack_string<'+'>, tail...>>
    {
            // lazy + operator found
            using new_node
                = nodes::basic_node<last_node, std::tuple<>, 1, std::numeric_limits<std::size_t>::max(), nodes::policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_PLUS */

#ifndef OPERATORS_ROUND_BRACKETS
#define OPERATORS_ROUND_BRACKETS

#include <nodes.hpp>

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'('>, tail...>>
    {
            // ( found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<nodes::grouping_node<subregex>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node,
                               std::tuple<pack_string<'('>, pack_string<'?'>, pack_string<':'>, tail...>>
    {
            // Non capturing group found
            using substring = extract_delimited_content_t<'(', ')', std::tuple<tail...>>;

            using subregex = typename tree_builder_helper<void, typename substring::result>::tree;
            using new_node = typename tree_builder_helper<nodes::basic_node<subregex, std::tuple<>>,
                                                          typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };
}// namespace e_regex

#endif /* OPERATORS_ROUND_BRACKETS */

#ifndef OPERATORS_SQUARE_BRACKETS_HPP
#define OPERATORS_SQUARE_BRACKETS_HPP

#include <terminals/exact_matcher.hpp>
#include <utilities/extract_delimited_content.hpp>

#ifndef TERMINALS_RANGE
#define TERMINALS_RANGE

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

    template<>
    struct rebuild_expression<terminal<>>
    {
            using string = pack_string<>;
    };

    template<char... chars, typename... tail>
    struct rebuild_expression<terminal<pack_string<chars...>, tail...>>
    {
            using string
                = merge_pack_strings_t<pack_string<chars...>,
                                       typename rebuild_expression<terminal<tail...>>::string>;
    };
}// namespace e_regex::terminals

#endif /* TERMINALS_COMMON_HPP */

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
                = add_child_t<last_node, nodes::basic_node<terminals::exact_matcher<head>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, char identifier, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<pack_string<'\\', identifier>, tail...>>
    {
            // Simple case, iterate

            using new_node = add_child_t<
                last_node,
                nodes::basic_node<terminals::terminal<pack_string<'\\', identifier>>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename start, typename end, typename... tail>
    struct square_bracker_tree_builder_helper<last_node, std::tuple<start, pack_string<'-'>, end, tail...>>
    {
            // Range found

            using new_node
                = add_child_t<last_node,
                              nodes::basic_node<terminals::range_terminal<start, end>, std::tuple<>>>;
            using tree =
                typename square_bracker_tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'['>, tail...>>
    {
            // [ found
            using substring = extract_delimited_content_t<'[', ']', std::tuple<tail...>>;

            using subregex =
                typename square_bracker_tree_builder_helper<nodes::basic_node<void, std::tuple<>>,
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
                typename square_bracker_tree_builder_helper<nodes::basic_node<void, std::tuple<>>,
                                                            typename substring::result>::tree;
            using new_node =
                typename tree_builder_helper<nodes::basic_node<nodes::negated_node<subregex>, std::tuple<>>,
                                             typename substring::remaining>::tree;

            using tree = add_child_t<last_node, new_node>;
    };

}// namespace e_regex

#endif /* OPERATORS_SQUARE_BRACKETS_HPP */

#ifndef OPERATORS_STAR
#define OPERATORS_STAR

namespace e_regex
{
    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, tail...>>
    {
            // greedy * operator found
            using new_node
                = nodes::basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), nodes::policy::GREEDY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'?'>, tail...>>
    {
            // lazy * operator found
            using new_node
                = nodes::basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), nodes::policy::LAZY>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };

    template<typename last_node, typename... tail>
    struct tree_builder_helper<last_node, std::tuple<pack_string<'*'>, pack_string<'+'>, tail...>>
    {
            // possessive * operator found
            using new_node
                = nodes::basic_node<last_node, std::tuple<>, 0, std::numeric_limits<std::size_t>::max(), nodes::policy::POSSESSIVE>;

            using tree = typename tree_builder_helper<new_node, std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_STAR */

#ifndef OPERATORS_START_ANCHOR
#define OPERATORS_START_ANCHOR

#include <nodes.hpp>
#include <terminals.hpp>

namespace e_regex
{
    template<typename... tail>
    struct tree_builder_helper<void, std::tuple<pack_string<'^'>, tail...>>
    {
            // Start anchor found

            using tree = typename tree_builder_helper<
                nodes::basic_node<terminals::anchors::start, std::tuple<>, 1, 1, nodes::policy::POSSESSIVE>,
                std::tuple<tail...>>::tree;
    };
}// namespace e_regex

#endif /* OPERATORS_START_ANCHOR */

#endif /* OPERATORS_HPP */

#ifndef TOKENIZER
#define TOKENIZER

#include <tuple>

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

#endif /* TREE_BUILDER_HPP */

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
