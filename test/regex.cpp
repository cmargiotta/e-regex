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

    REQUIRE(matcher("a").to_view() == "a");
    REQUIRE(!matcher("0").is_accepted());
}

TEST_CASE("Star operator")
{
    constexpr auto matcher = e_regex::match<"aa*">;

    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(matcher("a").to_view() == "a");

    constexpr auto aab = matcher("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab[0] == "aa");
}

TEST_CASE("Optional operator")
{
    constexpr auto matcher = e_regex::match<"a[a-f]?">;

    REQUIRE(matcher("aaa").to_view() == "aa");
    REQUIRE(matcher("a").is_accepted());
    REQUIRE(matcher("af").to_view() == "af");
}

TEST_CASE("Plus operator")
{
    constexpr auto matcher = e_regex::match<"aa+">;

    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(!matcher("a").is_accepted());

    constexpr auto aab = matcher("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.to_view() == "aa");
}

TEST_CASE("Round brackets")
{
    constexpr auto matcher = e_regex::match<"a(ab)+">;

    REQUIRE(!matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aab").to_view() == "aab");
    REQUIRE(matcher("aabab").to_view() == "aabab");
}

TEST_CASE("Braces")
{
    auto matcher = e_regex::match<"ab{2,10}c">;

    REQUIRE(matcher("abbc").is_accepted());
    REQUIRE(matcher("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher("abbbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher("abc").is_accepted());

    constexpr auto matcher1 = e_regex::match<"ab{2,}c">;

    REQUIRE(matcher1("abbc").is_accepted());
    REQUIRE(matcher1("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher1("abc").is_accepted());

    constexpr auto matcher2 = e_regex::match<"ab{2}c">;

    REQUIRE(matcher2("abbc").is_accepted());
    REQUIRE(!matcher2("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher2("abc").is_accepted());
}

TEST_CASE("Group matching")
{
    constexpr auto match = e_regex::match<"a(a(b))cd">("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabcd");
    REQUIRE(match[1] == "ab");
    REQUIRE(match[2] == "b");
}

TEST_CASE("Group matching order")
{
    constexpr auto match = e_regex::match<"a(a[a-g])+">("aabacad");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabacad");
    REQUIRE(match[1] == "ad");
}

TEST_CASE("Non-capturing round brackts")
{
    constexpr auto match = e_regex::match<"a(?:a(b))cd">("aabcdef");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabcd");
    REQUIRE(match[1] == "b");
}

TEST_CASE("Iterating matches")
{
    auto match = e_regex::match<"ab">("abaab");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "ab");

    match.next();
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "ab");
}

TEST_CASE("Square brackets")
{
    constexpr auto matcher = e_regex::match<"a[\\w\\-]+">;

    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aa-b").to_view() == "aa-b");
    REQUIRE(matcher("aab--ab").to_view() == "aab--ab");

    constexpr auto aabacb = matcher("12aaba12");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb.to_view() == "aaba");
}

TEST_CASE("Hex matching")
{
    constexpr auto matcher = e_regex::match<"a\\041">;

    REQUIRE(matcher("aA").to_view() == "aA");
    REQUIRE(!matcher("a").is_accepted());
}

TEST_CASE("Range matchers")
{
    constexpr auto matcher = e_regex::match<"a[a-fhm-o]+">;

    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aabfcno").to_view() == "aabfcno");
    REQUIRE(matcher("aabahb").to_view() == "aabahb");

    constexpr auto aabacb = matcher("baabazb");
    REQUIRE(aabacb.is_accepted());
    REQUIRE(aabacb[0] == "aaba");
}

TEST_CASE("Multiple branches")
{
    constexpr auto matcher = e_regex::match<"a|bc|cd|d">;

    auto match = matcher("abcd");
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "a");

    REQUIRE(match.next());
    REQUIRE(match[0] == "bc");

    REQUIRE(match.next());
    REQUIRE(match[0] == "d");

    REQUIRE(!match.next());
}

TEST_CASE("Negated matchers")
{
    constexpr auto matcher = e_regex::match<"a[^a-fh]+">;

    REQUIRE(matcher("axx").to_view() == "axx");
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(!matcher("aaf").is_accepted());
    REQUIRE(matcher("baggn").to_view() == "aggn");
}

TEST_CASE("Structured binding")
{
    constexpr auto matcher = e_regex::match<R"((\d+)-(\d+)-(\d+))">;

    auto [string, year, month, day] = matcher("2023-01-01");

    REQUIRE(string == "2023-01-01");
    REQUIRE(year == "2023");
    REQUIRE(month == "01");
    REQUIRE(day == "01");
}

TEST_CASE("General use")
{
    constexpr auto matcher = e_regex::match<R"([\w.\-]+@[\w\-]+\.[\w.]+)">;

    constexpr std::string_view email = "Test email <first.last@learnxinyminutes.com>";

    auto match = matcher(email);

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "first.last@learnxinyminutes.com");

    constexpr auto matcher1 = e_regex::match<"\"(.*)\"">;

    constexpr std::string_view test = "wrong \"match\"";

    auto match1 = matcher1(test);

    REQUIRE(match1.is_accepted());
    REQUIRE(match1[0] == "\"match\"");
    REQUIRE(match1[1] == "match");
}

TEST_CASE("Lazy and greedy plus")
{
    constexpr auto matcher_greedy = e_regex::match<"(a+)a">;

    std::string_view test = "aaaa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aaaa");
    REQUIRE(match_greedy[1] == "aaa");

    constexpr auto matcher_lazy = e_regex::match<"(a+?)a">;

    std::string_view test1 = "aaaa";

    auto match_lazy = matcher_lazy(test1);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "aa");
    REQUIRE(match_lazy[1] == "a");
}

TEST_CASE("Lazy, greedy and possessive optional")
{
    constexpr auto matcher_greedy = e_regex::match<"a?a">;

    std::string_view test = "aa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aa");

    constexpr auto matcher_lazy = e_regex::match<"a??a">;

    auto match_lazy = matcher_lazy(test);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "a");
    match_lazy.next();
    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "a");

    constexpr auto matcher_possessive = e_regex::match<"a?+a">;

    auto match_possessive = matcher_possessive(test);

    REQUIRE(match_possessive.is_accepted());
    REQUIRE(match_possessive[0] == "aa");

    REQUIRE(!matcher_possessive("a").is_accepted());
}

TEST_CASE("Lazy, greedy and possessive braces")
{
    constexpr auto matcher_greedy = e_regex::match<"(a{1,})a">;

    std::string_view test = "aaaa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aaaa");
    REQUIRE(match_greedy[1] == "aaa");

    constexpr auto matcher_lazy = e_regex::match<"(a{1,}?)a">;

    auto match_lazy = matcher_lazy(test);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "aa");
    REQUIRE(match_lazy[1] == "a");

    constexpr auto matcher_possessive = e_regex::match<"(a{1,}+)a">;

    auto match_possessive = matcher_possessive(test);

    REQUIRE(!match_possessive.is_accepted());
}