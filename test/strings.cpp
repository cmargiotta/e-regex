#include <catch2/catch.hpp>

#include <type_traits>

#include <static_string.hpp>
#include <tokenizer.hpp>
#include <utility.hpp>

TEST_CASE("Split")
{
    using namespace e_regex;

    using test   = decltype("bb(a)c)zz"_sstring);
    using tokens = e_regex::tokenize<test>;

    using expected = std::tuple<e_regex::pack_string<'b'>,
                                e_regex::pack_string<'b'>,
                                e_regex::pack_string<'('>,
                                e_regex::pack_string<'a'>,
                                e_regex::pack_string<')'>,
                                e_regex::pack_string<'c'>>;

    REQUIRE(std::is_same_v<typename extract_delimited_content_t<'(', ')', tokens>::result, expected>);
}

TEST_CASE("Brackets")
{
    using namespace e_regex;

    constexpr static_string regex {"bb(a)c)zz"};
    using tokens = e_regex::tokenize<regex>;

    using expected = std::tuple<e_regex::pack_string<'b'>,
                                e_regex::pack_string<'b'>,
                                e_regex::pack_string<'('>,
                                e_regex::pack_string<'a'>,
                                e_regex::pack_string<')'>,
                                e_regex::pack_string<'c'>>;

    REQUIRE(std::is_same_v<typename extract_delimited_content_t<'(', ')', tokens>::result, expected>);
}