#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include <e_regex.hpp>
#include <nodes.hpp>
#include <tokenizer.hpp>

TEST_CASE("Exact matchers merging")
{
    constexpr e_regex::static_string regex {"abcde1"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(std::is_same_v<
            e_regex::nodes::basic<void,
                                  std::tuple<e_regex::nodes::basic<
                                      e_regex::terminals::terminal<e_regex::pack_string<'a', 'b', 'c', 'd', 'e', '1'>>,
                                      std::tuple<>,
                                      1,
                                      1,
                                      e_regex::nodes::policy::GREEDY,
                                      false>>,
                                  1,
                                  1,
                                  e_regex::nodes::policy::GREEDY,
                                  false>,
            matcher>);
    REQUIRE(e_regex::nodes::get_expression_t<matcher>::string.to_view() == "abcde1");
}

TEST_CASE("Terminals merging")
{
    constexpr e_regex::static_string regex {R"(abc\defg)"};

    using test    = e_regex::build_pack_string_t<regex>;
    using matcher = typename e_regex::tree_builder<test>::tree;

    REQUIRE(std::is_same_v<
            e_regex::nodes::basic<void,
                                  std::tuple<e_regex::nodes::basic<
                                      e_regex::terminals::terminal<e_regex::pack_string<'a', 'b', 'c'>,
                                                                   e_regex::pack_string<'\\', 'd'>,
                                                                   e_regex::pack_string<'e', 'f', 'g'>>,
                                      std::tuple<>,
                                      1,
                                      1,
                                      e_regex::nodes::policy::GREEDY,
                                      false>>,
                                  1,
                                  1,
                                  e_regex::nodes::policy::GREEDY,
                                  false>,
            matcher>);

    REQUIRE(e_regex::nodes::get_expression_t<matcher>::string.to_view() == R"(abc\defg)");
}