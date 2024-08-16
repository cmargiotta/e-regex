#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include "e_regex.hpp"
#include "nodes.hpp"
#include "terminals/exact_matcher.hpp"
#include "tokenizer.hpp"
#include "utilities/admitted_set.hpp"
#include "utilities/pack_string.hpp"

TEST_CASE("Expression rebuilding")
{
    using matcher = typename e_regex::regex<R"(.{3,})">::ast;

    // The {3,} quantifier has been optimized to possessive
    REQUIRE(matcher::expression.to_view() == R"(.{3,}+)");
}

TEST_CASE("Admission set of a node")
{
    using matcher = typename e_regex::regex<R"(abc\defg)">::ast;

    REQUIRE(std::is_same_v<typename decltype(matcher::meta)::admission_set,
                           e_regex::admitted_set<char, 'a'>>);

    constexpr e_regex::static_string regex1 {R"([a-d]?z)"};

    using matcher1 =
        typename e_regex::tree_builder<e_regex::build_pack_string_t<regex1>>::tree;
    REQUIRE(
        std::is_same_v<typename decltype(matcher1::meta)::admission_set,
                       e_regex::admitted_set<char, 'a', 'b', 'c', 'd', 'z'>>);
}

TEST_CASE("Admission set intersection")
{
    using matcher  = typename e_regex::regex<R"(\d+)">::ast;
    using matcher1 = typename e_regex::regex<R"(\s+)">::ast;

    REQUIRE(e_regex::admitted_sets_intersection_t<
            typename decltype(matcher::meta)::admission_set,
            typename decltype(matcher1::meta)::admission_set>::empty);
}

TEST_CASE("Greedy backtracking removal")
{
    using matcher = typename e_regex::regex<R"(\d+\s+123)">::ast;

    REQUIRE(matcher::expression.to_view() == R"(\d++\s++123)");
}
