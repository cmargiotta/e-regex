#include <catch2/catch.hpp>

#include <type_traits>

#include <static_string.hpp>
#include <tokenizer.hpp>
#include <utility.hpp>

TEST_CASE("Split")
{
    using namespace e_regex;

    constexpr static_string regex {"a|b\\|c|d"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = tokenize<test>;
    using split_ = split_t<'|', tokens>;

    using expected = std::tuple<std::tuple<pack_string<'d'>>,
                                std::tuple<pack_string<'b'>, pack_string<'\\', '|'>, pack_string<'c'>>,
                                std::tuple<pack_string<'a'>>>;

    REQUIRE(std::is_same_v<split_, expected>);
}

TEST_CASE("Brackets")
{
    using namespace e_regex;

    constexpr static_string regex {"bb(a)c)zz"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = tokenize<test>;

    using expected
        = std::tuple<pack_string<'b'>, pack_string<'b'>, pack_string<'('>, pack_string<'a'>, pack_string<')'>, pack_string<'c'>>;

    REQUIRE(std::is_same_v<typename extract_delimited_content_t<'(', ')', tokens>::result, expected>);
}