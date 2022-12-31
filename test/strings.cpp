#include <catch2/catch.hpp>

#include <type_traits>

#include <static_string.hpp>
#include <tokenizer.hpp>
#include <utility.hpp>

template<typename T, T... data>
constexpr auto operator"" _sstring()
{
    return e_regex::static_string<data...> {};
}

TEST_CASE("Brackets")
{
    using namespace e_regex;

    using test   = decltype("bb(a)c)zz"_sstring);
    using tokens = e_regex::tokenize<test>;

    using expected = std::tuple<e_regex::static_string<'b'>,
                                e_regex::static_string<'b'>,
                                e_regex::static_string<'('>,
                                e_regex::static_string<'a'>,
                                e_regex::static_string<')'>,
                                e_regex::static_string<'c'>>;

    REQUIRE(std::is_same_v<typename extract_delimited_content_t<'(', ')', tokens>::result, expected>);
}