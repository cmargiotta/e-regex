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
    constexpr auto matcher = e_regex::match<"\\w">;

    REQUIRE(matcher("a").is_accepted());
    REQUIRE(!matcher("0").is_accepted());
}

TEST_CASE("Star operator")
{
    constexpr auto matcher = e_regex::match<"aa*">;

    REQUIRE(matcher("aaa").is_accepted());
    REQUIRE(matcher("a").is_accepted());

    auto aab = matcher("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.get_group(0) == "aa");
}

TEST_CASE("Optional operator")
{
    constexpr auto matcher = e_regex::match<"a[a-f]?">;

    REQUIRE(matcher("aaa").is_accepted());
    REQUIRE(matcher("a").is_accepted());
    REQUIRE(matcher("af").is_accepted());
}

TEST_CASE("Plus operator")
{
    constexpr auto matcher = e_regex::match<"aa+">;

    REQUIRE(matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());

    auto aab = matcher("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.get_group(0) == "aa");
}

TEST_CASE("Round brackets")
{
    constexpr auto matcher = e_regex::match<"a(ab)+">;

    REQUIRE(!matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aab").is_accepted());
    REQUIRE(matcher("aabab").is_accepted());
}

TEST_CASE("Group matching")
{
    auto match = e_regex::match<"a(a(b))cd">("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "aabcd");
    REQUIRE(match.get_group(1) == "ab");
    REQUIRE(match.get_group(2) == "b");
}

TEST_CASE("Non-capturing round brackts")
{
    auto match = e_regex::match<"a(?:a(b))cd">("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "aabcd");
    REQUIRE(match.get_group(1) == "b");
}

TEST_CASE("Iterating matches")
{
    auto match = e_regex::match<"ab">("abaab");

    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "ab");

    match.next();
    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "ab");
}

TEST_CASE("Square brackets")
{
    constexpr auto matcher = e_regex::match<"a[\\w\\-]+">;

    REQUIRE(matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aa-b").is_accepted());
    REQUIRE(matcher("aab--ab").is_accepted());

    auto aabacb = matcher("12aaba12");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb.to_view() == "aaba");
}

TEST_CASE("Range matchers")
{
    constexpr auto matcher = e_regex::match<"a[a-fhm-o]+">;

    REQUIRE(matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aabfcno").is_accepted());
    REQUIRE(matcher("aabahb").is_accepted());

    auto aabacb = matcher("baabazb");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb.get_group(0) == "aaba");
}

TEST_CASE("Multiple branches")
{
    constexpr auto matcher = e_regex::match<"a|bc|cd|d">;

    auto match = matcher("abcd");
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
    constexpr auto matcher = e_regex::match<"a[^a-fh]+">;

    REQUIRE(matcher("axx").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(!matcher("aaf").is_accepted());
    REQUIRE(matcher("baggn").is_accepted());
}

TEST_CASE("General use")
{
    constexpr auto matcher = e_regex::match<R"([\w.\-]+@[\w\-]+\.[\w.]+)">;

    constexpr std::string_view email = "Test email <first.last@learnxinyminutes.com>";

    auto match = matcher(email);
    REQUIRE(match.is_accepted());
    REQUIRE(match.get_group(0) == "first.last@learnxinyminutes.com");
}