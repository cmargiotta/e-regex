#include <catch2/catch_test_macros.hpp>

#include <type_traits>

#include "static_string.hpp"
#include "tokenizer.hpp"
#include "utilities/extract_delimited_content.hpp"
#include "utilities/pack_string_to_number.hpp"
#include "utilities/split.hpp"

TEST_CASE("Split")
{
    using e_regex::pack_string;

    constexpr e_regex::static_string regex {"a|b\\|c|d"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = e_regex::tokenizer_t<test>;
    using split_ = e_regex::split_t<'|', tokens>;

    using expected = std::tuple<std::tuple<pack_string<'a'>>,
                                std::tuple<pack_string<'b'>, pack_string<'\\', '|'>, pack_string<'c'>>,
                                std::tuple<pack_string<'d'>>>;

    REQUIRE(std::is_same_v<split_, expected>);
}

TEST_CASE("Brackets")
{
    using e_regex::pack_string;

    constexpr e_regex::static_string regex {"bb(a)c)zz"};

    using test   = e_regex::build_pack_string_t<regex>;
    using tokens = e_regex::tokenizer_t<test>;

    using expected
        = std::tuple<pack_string<'b'>, pack_string<'b'>, pack_string<'('>, pack_string<'a'>, pack_string<')'>, pack_string<'c'>>;

    REQUIRE(
        std::is_same_v<typename e_regex::extract_delimited_content_t<'(', ')', tokens>::result, expected>);
}

TEST_CASE("pack_string to number")
{
    constexpr e_regex::static_string regex {"150"};

    using test            = e_regex::build_pack_string_t<regex>;
    constexpr auto number = e_regex::pack_string_to_number<test>::value;

    REQUIRE(number == 150);
}