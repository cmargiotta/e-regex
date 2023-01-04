#include <catch2/catch.hpp>

#include <type_traits>

#include <e_regex.hpp>
#include <tokenizer.hpp>

TEST_CASE("Tokenization")
{
    constexpr e_regex::static_string regex {R"(bb(a\)c)zz)"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = e_regex::tokenize<test>;

    using expected = std::tuple<e_regex::pack_string<'b'>,
                                e_regex::pack_string<'b'>,
                                e_regex::pack_string<'('>,
                                e_regex::pack_string<'a'>,
                                e_regex::pack_string<'\\', ')'>,
                                e_regex::pack_string<'c'>,
                                e_regex::pack_string<')'>,
                                e_regex::pack_string<'z'>,
                                e_regex::pack_string<'z'>>;

    REQUIRE(std::is_same_v<tokens, expected>);
}

TEST_CASE("Construction")
{
    constexpr e_regex::static_string regex {"\\w"};

    REQUIRE(e_regex::match<regex>("a").is_accepted());
    REQUIRE(!e_regex::match<regex>("0").is_accepted());
}

TEST_CASE("Star operator")
{
    constexpr e_regex::static_string regex {"aa*"};

    REQUIRE(e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(e_regex::match<regex>("a").is_accepted());

    auto aab = e_regex::match<regex>("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.get_group(0) == "aa");
}

TEST_CASE("Optional operator")
{
    constexpr e_regex::static_string regex {"a[a-f]?"};

    REQUIRE(e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(e_regex::match<regex>("a").is_accepted());
    REQUIRE(e_regex::match<regex>("af").is_accepted());
}

TEST_CASE("Plus operator")
{
    constexpr e_regex::static_string regex {"aa+"};

    REQUIRE(e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(!e_regex::match<regex>("a").is_accepted());

    auto aab = e_regex::match<regex>("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.get_group(0) == "aa");
}

TEST_CASE("Round brackets")
{
    constexpr e_regex::static_string regex {"a(ab)+"};

    REQUIRE(!e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(!e_regex::match<regex>("a").is_accepted());
    REQUIRE(e_regex::match<regex>("aab").is_accepted());
    REQUIRE(e_regex::match<regex>("aabab").is_accepted());
}

TEST_CASE("Group matching")
{
    constexpr e_regex::static_string regex {"a(a(b))cd"};

    auto match = e_regex::match<regex>("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "aabcd");
    REQUIRE(match.get_group(1) == "ab");
    REQUIRE(match.get_group(2) == "b");
}

TEST_CASE("Non-capturing round brackts")
{
    constexpr e_regex::static_string regex {"a(?:a(b))cd"};

    auto match = e_regex::match<regex>("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "aabcd");
    REQUIRE(match.get_group(1) == "b");
}

TEST_CASE("Iterating matches")
{
    constexpr e_regex::static_string regex {"ab"};

    auto match = e_regex::match<regex>("abaab");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "ab");

    match.next();
    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "ab");
}

TEST_CASE("Square brackets")
{
    constexpr e_regex::static_string regex {"a[\\w\\-]+"};

    REQUIRE(e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(!e_regex::match<regex>("a").is_accepted());
    REQUIRE(e_regex::match<regex>("aa-b").is_accepted());
    REQUIRE(e_regex::match<regex>("aab--ab").is_accepted());

    auto aabacb = e_regex::match<regex>("12aaba12");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb.to_view() == "aaba");
}

TEST_CASE("Range matchers")
{
    constexpr e_regex::static_string regex {"a[a-fhm-o]+"};

    REQUIRE(e_regex::match<regex>("aaa").is_accepted());
    REQUIRE(!e_regex::match<regex>("a").is_accepted());
    REQUIRE(e_regex::match<regex>("aabfcno").is_accepted());
    REQUIRE(e_regex::match<regex>("aabahb").is_accepted());

    auto aabacb = e_regex::match<regex>("baabazb");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb.get_group(0) == "aaba");
}

TEST_CASE("Multiple branches")
{
    constexpr e_regex::static_string regex {"a|bc|cd|d"};

    auto match = e_regex::match<regex>("abcd");
    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "a");

    REQUIRE(match.next());
    REQUIRE(match.get_group(0) == "bc");

    REQUIRE(match.next());
    REQUIRE(match.get_group(0) == "d");

    REQUIRE(!match.next());
}

TEST_CASE("Negated matchers")
{
    constexpr e_regex::static_string regex {"a[^a-fh]+"};

    REQUIRE(e_regex::match<regex>("axx").is_accepted());
    REQUIRE(!e_regex::match<regex>("a").is_accepted());
    REQUIRE(!e_regex::match<regex>("aaf").is_accepted());
    REQUIRE(e_regex::match<regex>("baggn").is_accepted());
}

TEST_CASE("General use")
{
    constexpr e_regex::static_string regex {R"([\w.\-]+@[\w\-]+\.[\w.]+)"};

    constexpr std::string_view email = "Test email <first.last@learnxinyminutes.com>";

    auto match = e_regex::match<regex>(email);
    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "first.last@learnxinyminutes.com");
}