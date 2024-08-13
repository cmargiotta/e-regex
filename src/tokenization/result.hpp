#ifndef E_REGEX_TOKENIZATION_RESULT_HPP_
#define E_REGEX_TOKENIZATION_RESULT_HPP_

// #include <concepts>
// #include <string_view>
// #include <tuple>
// #include <type_traits>

// #include "iterator.hpp"
// #include "token.hpp"
// #include "utilities/literal_string_view.hpp"
// #include "utilities/pack_string.hpp"
// #include "utilities/static_string.hpp"

// namespace e_regex
// {
//     template<typename T, auto size>
//     struct token
//     {
//             T                   type;
//             static_string<size> matcher;

//             constexpr token(T type, const char (&matcher)[size])
//                 : type {type}, matcher {matcher}
//             {}
//     };

//     template<auto size>
//     struct separator
//     {
//             static_string<size> matcher;

//             constexpr separator(const char (&matcher)[size])
//                 : matcher {matcher}
//             {}
//     };

// } // namespace e_regex

// namespace e_regex::tokenization
// {
//     template<auto matcher, auto separator_verifier, std::array
//     type_map, typename Char_Type = char> class result
//     {
//         private:
//             using match_result = decltype(matcher(
//                 std::declval<literal_string_view<>>()));

//         public:
//             using token_class = decltype(type_map)::value_type;
//             using token_type
//                 = token<token_class,
//                 literal_string_view<Char_Type>>;
//             using iter
//                 = iterator<match_result, token_type,
//                 separator_verifier, type_map>;

//         private:
//             match_result regex_result;

//         public:
//             constexpr explicit result(literal_string_view<>
//             expression)
//                 : regex_result {matcher(expression)} {};

//             constexpr auto begin() const noexcept -> iter
//             {
//                 return iter {regex_result};
//             }

//             constexpr auto end() const noexcept -> iter
//             {
//                 iter res {regex_result};
//                 res.invalidate();

//                 return res;
//             }

//             constexpr auto count() const noexcept
//             {
//                 std::size_t result = 0;
//                 auto        i      = begin();

//                 while (i)
//                 {
//                     ++result;
//                     ++i;
//                 }

//                 return result;
//             }
//     };

//     template<typename T>
//     struct is_token : public std::false_type
//     {};

//     template<typename T, auto size>
//     struct is_token<e_regex::token<T, size>> : public
//     std::true_type
//     {};

//     template<typename T>
//     concept is_token_c = is_token<T>::value;

//     template<typename T>
//     struct is_separator : public std::false_type
//     {};

//     template<auto size>
//     struct is_separator<e_regex::separator<size>> : public
//     std::true_type
//     {};

//     template<typename T>
//     concept is_separator_c = is_separator<T>::value;

//     template<typename T>
//     concept token_definition = is_separator_c<T> || is_token_c<T>;

//     template<typename T, std::size_t N, std::size_t... I>
//     constexpr auto append_aux(std::array<T, N> a,
//                               T                t,
//                               std::index_sequence<I...>)
//         -> std::array<T, N + 1>
//     {
//         return std::array<T, N + 1> {a[I]..., t};
//     }

//     template<typename T, std::size_t N>
//     constexpr auto append(std::array<T, N> a, T t)
//         -> std::array<T, N + 1>
//     {
//         return append_aux(a, t, std::make_index_sequence<N>());
//     }

//     template<typename T, typename T1>
//     constexpr auto append(std::array<T, 0> /*unused*/, T1 t)
//         -> std::array<T1, 1>
//     {
//         return std::array {t};
//     }

//     template<typename separators, typename tokens, auto type_map,
//     token_definition auto... input> struct token_data_parser;

//     template<typename separators,
//              typename tokens,
//              auto                type_map,
//              is_separator_c auto current,
//              token_definition auto... input>
//     struct token_data_parser<separators, tokens, type_map, current,
//     input...>
//     {
//             using new_separators = concatenate_pack_strings_t<
//                 pack_string<'|'>,
//                 separators,
//                 build_pack_string_t<static_string
//                 {current.matcher}>>;

//             using type
//                 = token_data_parser<new_separators, tokens,
//                 type_map, input...>;

//             using token_matcher = typename type::token_matcher;
//             using separator_matcher = typename
//             type::separator_matcher; static constexpr const auto&
//             types = type::types;
//     };

//     template<typename separators,
//              typename tokens,
//              auto            type_map,
//              is_token_c auto current,
//              token_definition auto... input>
//     struct token_data_parser<separators, tokens, type_map, current,
//     input...>
//     {
//             using new_tokens = concatenate_pack_strings_t<
//                 pack_string<'|'>,
//                 tokens,
//                 merge_pack_strings_t<
//                     build_pack_string_t<static_string {"("}>,
//                     build_pack_string_t<static_string
//                     {current.matcher}>,
//                     build_pack_string_t<static_string {")"}>>>;

//             static constexpr auto new_type_map
//                 = append(type_map, current.type);

//             using type
//                 = token_data_parser<separators, new_tokens,
//                 new_type_map, input...>;

//             using token_matcher = typename type::token_matcher;
//             using separator_matcher = typename
//             type::separator_matcher; static constexpr auto& types =
//             type::types;
//     }; // namespace e_regex::tokenization

//     template<typename separators, typename tokens, auto type_map>
//     struct token_data_parser<separators, tokens, type_map>
//     {
//             using token_matcher          = tokens;
//             using separator_matcher      = separators;
//             static constexpr auto& types = type_map;
//     };

//     template<token_definition auto... tokens>
//     struct result_builder
//     {
//             using data = token_data_parser<pack_string<>,
//                                            pack_string<>,
//                                            std::array<int, 0> {},
//                                            tokens...>;
//     };
// } // namespace e_regex::tokenization

#endif /* E_REGEX_TOKENIZATION_RESULT_HPP_*/
