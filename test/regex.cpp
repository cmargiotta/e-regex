#include <catch2/catch.hpp>

#include <type_traits>

#include <e_regex.hpp>
#include <tokenizer.hpp>

template<typename T, T... data>
constexpr auto operator"" _sstring()
{
    return e_regex::static_string<data...> {};
}

TEST_CASE("Tokenization")
{
    using test   = decltype(R"(bb(a\)c)zz)"_sstring);
    using tokens = e_regex::tokenize<test>;

    using expected = std::tuple<e_regex::static_string<'b'>,
                                e_regex::static_string<'b'>,
                                e_regex::static_string<'('>,
                                e_regex::static_string<'a'>,
                                e_regex::static_string<'\\', ')'>,
                                e_regex::static_string<'c'>,
                                e_regex::static_string<')'>,
                                e_regex::static_string<'z'>,
                                e_regex::static_string<'z'>>;

    REQUIRE(std::is_same_v<tokens, expected>);
}

TEST_CASE("Construction")
{
    static auto regex = "aaa"_regex;

    REQUIRE(regex.match("aaa").is_accepted());
    REQUIRE(!regex.match("aab").is_accepted());
}

TEST_CASE("Star operator")
{
    static auto regex = "aa*"_regex;

    REQUIRE(regex.match("aaa").is_accepted());
    REQUIRE(regex.match("a").is_accepted());

    auto aab = regex.match("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab[0] == "aa");
}

TEST_CASE("Plus operator")
{
    static auto regex = "aa+"_regex;

    REQUIRE(regex.match("aaa").is_accepted());
    REQUIRE(!regex.match("a").is_accepted());

    auto aab = regex.match("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab[0] == "aa");
}

TEST_CASE("Round brackets")
{
    static auto regex = "a(ab)+"_regex;

    REQUIRE(!regex.match("aaa").is_accepted());
    REQUIRE(!regex.match("a").is_accepted());
    REQUIRE(regex.match("aab").is_accepted());
    REQUIRE(regex.match("aabab").is_accepted());
}

TEST_CASE("Group matching")
{
    static auto regex = "a(a(b))cd"_regex;

    auto match = regex.match("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabcd");
    REQUIRE(match[1] == "ab");
    REQUIRE(match[2] == "b");
}

TEST_CASE("Square brackets")
{
    static auto regex = "a[ab]+"_regex;

    REQUIRE(regex.match("aaa").is_accepted());
    REQUIRE(!regex.match("a").is_accepted());
    REQUIRE(regex.match("aab").is_accepted());
    REQUIRE(regex.match("aabab").is_accepted());

    auto aabacb = regex.match("aabacb");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb[0] == "aaba");
}

TEST_CASE("Range matchers")
{
    static auto regex = "a[a-fhm-o]+"_regex;

    REQUIRE(regex.match("aaa").is_accepted());
    REQUIRE(!regex.match("a").is_accepted());
    REQUIRE(regex.match("aabfcno").is_accepted());
    REQUIRE(regex.match("aabahb").is_accepted());

    auto aabacb = regex.match("aabazb");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb[0] == "aaba");
}

TEST_CASE("Negated matchers")
{
    static auto regex = "a[^a-fh]+"_regex;

    REQUIRE(regex.match("axx").is_accepted());
    REQUIRE(!regex.match("a").is_accepted());
    REQUIRE(!regex.match("aaf").is_accepted());
    REQUIRE(regex.match("aggn").is_accepted());
}