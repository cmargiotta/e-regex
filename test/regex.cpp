#include <catch2/catch_test_macros.hpp>

#include <string_view>
#include <type_traits>

#include <e_regex.hpp>
#include <tokenizer.hpp>

#include "match_result.hpp"

TEST_CASE("Tokenization")
{
    constexpr e_regex::static_string regex {R"(bb(a\)c)zz)"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = e_regex::tokenizer_t<test>;

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
    constexpr e_regex::regex<"\\w"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("a").to_view() == "a");
    REQUIRE(!matcher(" ").is_accepted());
}

TEST_CASE("Star operator")
{
    constexpr e_regex::regex<"aa*"> matcher;

    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(matcher("a").to_view() == "a");

    constexpr auto aab = matcher("aab");

    REQUIRE(matcher.groups == 0);
    REQUIRE(aab.is_accepted());
    REQUIRE(aab[0] == "aa");
}

TEST_CASE("Optional operator")
{
    constexpr e_regex::regex<"a[a-f]?"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("aaa").to_view() == "aa");
    REQUIRE(matcher("a").is_accepted());
    REQUIRE(matcher("af").to_view() == "af");
}

TEST_CASE("Plus operator")
{
    constexpr e_regex::regex<"aa+"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("aaa").to_view() == "aaa");
    REQUIRE(!matcher("a").is_accepted());

    constexpr auto aab = matcher("aab");
    REQUIRE(aab.is_accepted());
    REQUIRE(aab.to_view() == "aa");
}

TEST_CASE("Round brackets")
{
    constexpr e_regex::regex<"a(ab)+"> matcher;

    REQUIRE(matcher.groups == 1);
    REQUIRE(!matcher("aaa").is_accepted());
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(matcher("aab").to_view() == "aab");
    REQUIRE(matcher("aabab").to_view() == "aabab");
}

TEST_CASE("Braces")
{
    constexpr e_regex::regex<"ab{2,10}c"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("abbc").is_accepted());
    REQUIRE(matcher("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher("abbbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher("abc").is_accepted());

    constexpr e_regex::regex<"ab{2,}c"> matcher1;

    REQUIRE(matcher1.groups == 0);
    REQUIRE(matcher1("abbc").is_accepted());
    REQUIRE(matcher1("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher1("abc").is_accepted());

    constexpr e_regex::regex<"ab{2}c"> matcher2;

    REQUIRE(matcher2.groups == 0);
    REQUIRE(matcher2("abbc").is_accepted());
    REQUIRE(!matcher2("abbbbbbbbbbc").is_accepted());
    REQUIRE(!matcher2("abc").is_accepted());
}

TEST_CASE("Nested group matching")
{
    constexpr auto matcher = e_regex::regex<"a(a(b))cd"> {};
    // decltype(matcher)::ast a       = 10;
    // static_assert(matcher.groups == 2);
    constexpr auto match = matcher("aabcdef");

    REQUIRE(matcher.groups == 2);
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabcd");
    REQUIRE(match[1] == "ab");
    REQUIRE(match[2] == "b");
}

TEST_CASE("Branched group matching")
{
    constexpr e_regex::regex<"a(a|b)+"> matcher;

    REQUIRE(matcher("aa").is_accepted());
    REQUIRE(matcher("ab").is_accepted());
    REQUIRE(matcher("abaab").is_accepted());

    constexpr auto match = matcher("aab");
    REQUIRE(match[0] == "aab");
    REQUIRE(match[1] == "b");
}

TEST_CASE("Ranged square bracket - 1")
{
    constexpr e_regex::regex<R"((10[0-7]0))"> matcher;

    auto match = matcher("274.06 102	1000	");
    auto full  = match;

    REQUIRE(matcher.groups == 1);
    REQUIRE(full.to_view() == "1000");
}

TEST_CASE("Group matching order")
{
    constexpr auto match
        = e_regex::regex<"a(a[a-g])+">::match("aabacad");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabacad");
    REQUIRE(match[1] == "ad");
}

TEST_CASE("Group matching with more branches")
{
    constexpr e_regex::regex<R"(\w(\w+)|(\d+))"> matcher;

    auto match                 = matcher("abc");
    auto [full, first, second] = match;

    REQUIRE(matcher.groups == 2);
    REQUIRE(full == "abc");
    REQUIRE(first == "bc");
    REQUIRE(second.empty());

    auto [full1, first1, second1] = matcher("-2-");

    REQUIRE(full1 == "2");
    REQUIRE(first1.empty());
    REQUIRE(second1 == "2");
}

TEST_CASE("IP matching")
{
    constexpr e_regex::regex<"(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9])\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9])">
        matcher;

    auto match = matcher("| | 66.249.64.13 | aaaaaaaa test");

    REQUIRE(match.is_accepted());
    REQUIRE(match.to_view() == "66.249.64.13");
}

TEST_CASE("Iteration on group matching with more branches")
{
    constexpr e_regex::regex<R"([a-z]([a-z]+)|(\d+))"> matcher;

    auto match                 = matcher("abc123ab");
    auto [full, first, second] = match;

    REQUIRE(matcher.groups == 2);
    REQUIRE(full == "abc");
    REQUIRE(first == "bc");
    REQUIRE(second.empty());

    match.next();
    auto [full1, first1, second1] = match;

    REQUIRE(full1 == "123");
    REQUIRE(first1.empty());
    REQUIRE(second1 == "123");

    match.next();
    auto [full2, first2, second2] = match;

    REQUIRE(full2 == "ab");
    REQUIRE(first2 == "b");
    REQUIRE(second2.empty());
}

TEST_CASE("Branches collisions handling")
{
    constexpr e_regex::regex<"aaa|a+"> matcher;

    auto res = matcher("aaaa");

    REQUIRE(matcher.groups == 0);
    REQUIRE(res.to_view() == "aaa");
}

TEST_CASE("Non-capturing round brackts")
{
    constexpr e_regex::regex<"a(?:a(b))cd"> matcher;
    constexpr auto match = matcher("aabcdef");

    REQUIRE(matcher.groups == 1);
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "aabcd");
    REQUIRE(match[1] == "b");
}

TEST_CASE("Iterating matches")
{
    auto match = e_regex::regex<"ab"> {}("abaab");

    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "ab");

    match.next();
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "ab");
}

TEST_CASE("Square brackets")
{
    constexpr e_regex::regex<"a[a-z\\-]+"> matcher;

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
    constexpr e_regex::regex<"a\\x41"> matcher;

    REQUIRE(matcher("aA").to_view() == "aA");
    REQUIRE(!matcher("a").is_accepted());
}

TEST_CASE("Hex string matching")
{
    constexpr e_regex::regex<"a\\x{414141}"> matcher;

    REQUIRE(matcher("aAAA").to_view() == "aAAA");
    REQUIRE(!matcher("aAA").is_accepted());
}

TEST_CASE("Octal matching")
{
    constexpr e_regex::regex<"a\\101"> matcher;

    REQUIRE(matcher("aA").to_view() == "aA");
    REQUIRE(!matcher("a").is_accepted());
}

TEST_CASE("Octal string matching")
{
    constexpr e_regex::regex<"a\\o{101101101}"> matcher;

    REQUIRE(matcher("aAAA").to_view() == "aAAA");
    REQUIRE(!matcher("aAA").is_accepted());
}

TEST_CASE("Start anchor")
{
    constexpr e_regex::regex<"^a"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("abc").is_accepted());
    REQUIRE(!matcher("bc").is_accepted());
}

TEST_CASE("End anchor")
{
    constexpr e_regex::regex<"a$"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("a").is_accepted());
    REQUIRE(!matcher("abc").is_accepted());
    REQUIRE(matcher("aabca").is_accepted());
}

TEST_CASE("Range matchers")
{
    constexpr e_regex::regex<"a[a-fhm-o]+"> matcher;

    REQUIRE(matcher.groups == 0);
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
    constexpr e_regex::regex<"a|bc|cd|d"> matcher;

    auto match = matcher("abcd");

    REQUIRE(matcher.groups == 0);
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
    constexpr e_regex::regex<"a[^a-fh]+"> matcher;

    REQUIRE(matcher.groups == 0);
    REQUIRE(matcher("axx").to_view() == "axx");
    REQUIRE(!matcher("a").is_accepted());
    REQUIRE(!matcher("aaf").is_accepted());
    REQUIRE(matcher("baggn").to_view() == "aggn");
}

TEST_CASE("Structured binding")
{
    constexpr e_regex::regex<R"((\d+)-(\d+)-(\d+))"> matcher;
    auto [string, year, month, day] = matcher("2023-01-01");

    REQUIRE(matcher.groups == 3);
    REQUIRE(string == "2023-01-01");
    REQUIRE(year == "2023");
    REQUIRE(month == "01");
    REQUIRE(day == "01");
}

TEST_CASE("General use")
{
    constexpr e_regex::regex<R"([\w.\-]+@[\w\-]+\.[\w.]+)"> matcher;

    constexpr std::string_view email
        = "Test email <red-langNO_SPAM@googlegroups.com>";

    auto match = matcher(email);

    REQUIRE(matcher.groups == 0);
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "red-langNO_SPAM@googlegroups.com");

    constexpr e_regex::regex<"\"(.*)\""> matcher1;

    constexpr std::string_view test = "wrong \"match\"";

    auto match1 = matcher1(test);

    REQUIRE(match1.is_accepted());
    REQUIRE(match1[0] == "\"match\"");
    REQUIRE(match1[1] == "match");
}

TEST_CASE("Multiline input")
{
    constexpr e_regex::regex<R"([\w.\-]+@[\w\-]+\.[\w.]+)"> matcher;

    constexpr std::string_view email = "\
ABCDTESTTESTEST fir@aaaaaa@aaa     \
Test email <first.last@learnxinyminutes.com>";

    auto match = matcher(email);

    REQUIRE(matcher.groups == 0);
    REQUIRE(match.is_accepted());
    REQUIRE(match[0] == "first.last@learnxinyminutes.com");
}

TEST_CASE("Iterating")
{
    constexpr e_regex::regex<"\\dab"> matcher;

    std::string_view test = "aaa1abaaaaaaaccc2ab\
//  aba3ab4ab5ab";

    auto counter = 0;
    for (auto match: matcher(test))
    {
        if (!match.empty())
        {
            ++counter;
        }
    }

    REQUIRE(counter == 5);
}

TEST_CASE("Lazy and greedy plus")
{
    constexpr e_regex::regex<"(a+)a"> matcher_greedy;
    std::string_view                  test = "aaaa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(matcher_greedy.groups == 1);
    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aaaa");
    REQUIRE(match_greedy[1] == "aaa");

    constexpr e_regex::regex<"(a+?)a"> matcher_lazy;

    std::string_view test1 = "aaaa";

    auto match_lazy = matcher_lazy(test1);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "aa");
    REQUIRE(match_lazy[1] == "a");
}

TEST_CASE("Lazy, greedy and possessive optional")
{
    constexpr e_regex::regex<"a?a"> matcher_greedy;

    std::string_view test = "aa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aa");

    constexpr e_regex::regex<"a??a"> matcher_lazy;
    auto                             match_lazy = matcher_lazy(test);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "a");
    match_lazy.next();
    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "a");

    constexpr e_regex::regex<"a?+a"> matcher_possessive;

    auto match_possessive = matcher_possessive(test);

    REQUIRE(match_possessive.is_accepted());
    REQUIRE(match_possessive[0] == "aa");

    REQUIRE(!matcher_possessive("a").is_accepted());
}

TEST_CASE("Lazy, greedy and possessive braces")
{
    constexpr e_regex::regex<"(a{1,})a"> matcher_greedy;

    std::string_view test = "aaaa";

    auto match_greedy = matcher_greedy(test);

    REQUIRE(match_greedy.is_accepted());
    REQUIRE(match_greedy[0] == "aaaa");
    REQUIRE(match_greedy[1] == "aaa");

    constexpr e_regex::regex<"(a{1,}?)a"> matcher_lazy;

    auto match_lazy = matcher_lazy(test);

    REQUIRE(match_lazy.is_accepted());
    REQUIRE(match_lazy[0] == "aa");
    REQUIRE(match_lazy[1] == "a");

    constexpr e_regex::regex<"(a{1,}+)a"> matcher_possessive;

    auto match_possessive = matcher_possessive(test);

    REQUIRE(!match_possessive.is_accepted());
}

TEST_CASE("Intersection")
{
    constexpr e_regex::regex<"\\dabcd">      matcher;
    constexpr e_regex::regex<"[a-z]abcd">    matcher1;
    constexpr e_regex::regex<"\\d*\\w+abcd"> matcher2;

    REQUIRE(matcher.is_independent(matcher1));
    REQUIRE(!matcher.is_independent(matcher2));
    REQUIRE(!matcher1.is_independent(matcher2));
}
