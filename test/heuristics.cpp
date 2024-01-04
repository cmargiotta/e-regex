#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include "e_regex.hpp"
#include "nodes.hpp"
#include "tokenizer.hpp"
#include "utilities/admitted_set.hpp"

TEST_CASE("Exact matchers merging")
{
    constexpr e_regex::static_string regex {"abcde1"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(
        std::is_same_v<e_regex::nodes::simple<void,
                                              e_regex::nodes::simple<e_regex::terminals::terminal<
                                                  e_regex::pack_string<'a', 'b', 'c', 'd', 'e', '1'>>>>,
                       matcher>);

    REQUIRE(matcher::expression::string.to_view() == "abcde1");
}

TEST_CASE("Terminals merging")
{
    constexpr e_regex::static_string regex {R"(abc\defg)"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(std::is_same_v<e_regex::nodes::simple<void,
                                                  e_regex::nodes::simple<e_regex::terminals::terminal<
                                                      e_regex::pack_string<'a', 'b', 'c'>,
                                                      e_regex::pack_string<'\\', 'd'>,
                                                      e_regex::pack_string<'e', 'f', 'g'>>>>,
                           matcher>);

    REQUIRE(matcher::expression::string.to_view() == R"(abc\defg)");
}

TEST_CASE("Expression rebuilding")
{
    constexpr e_regex::static_string regex {R"(.{3,})"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(matcher::expression::string.to_view() == R"(.{3,})");
}

TEST_CASE("Admission set of a node")
{
    constexpr e_regex::static_string regex {R"(abc\defg)"};

    using matcher = typename e_regex::tree_builder<e_regex::build_pack_string_t<regex>>::tree;
    REQUIRE(std::is_same_v<matcher::admitted_first_chars, e_regex::admitted_set<char, 'a'>>);

    constexpr e_regex::static_string regex1 {R"([a-d]?z)"};

    using matcher1 = typename e_regex::tree_builder<e_regex::build_pack_string_t<regex1>>::tree;
    REQUIRE(
        std::is_same_v<matcher1::admitted_first_chars, e_regex::admitted_set<char, 'a', 'b', 'c', 'd', 'z'>>);
}

TEST_CASE("Admission set intersection")
{
    constexpr e_regex::static_string regex {R"(\d+)"};
    constexpr e_regex::static_string regex1 {R"(\s+)"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    using test1    = e_regex::build_pack_string_t<regex1>;
    using matcher1 = typename e_regex::tree_builder<test1>::tree;

    REQUIRE(e_regex::admitted_sets_intersection_t<typename matcher::admitted_first_chars,
                                                  typename matcher1::admitted_first_chars>::empty);
}

TEST_CASE("Greedy backtracking removal")
{
    constexpr e_regex::static_string regex {R"(\d+\s+123)"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(matcher::expression::string.to_view() == R"(\d++\s++123)");
}