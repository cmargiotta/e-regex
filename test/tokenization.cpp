#include <catch2/catch.hpp>

#include <vector>

#include <e_regex.hpp>
#include <tokenizer.hpp>

TEST_CASE("Tokenization by space")
{
    constexpr auto tokenizer = e_regex::tokenize<"[^\\s]+", "\\s">;

    constexpr auto res = tokenizer("a abc def");

    std::vector<std::string_view> tokens;

    for (auto token: res)
    {
        tokens.push_back(token);
    }

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == "abc");
    REQUIRE(tokens[2] == "def");
}

// TEST_CASE("Tokenization at compile time")
// {
//     constexpr auto tokenizer = e_regex::tokenize<"[^\\s]+", "\\s">;

//     constexpr auto res = tokenizer("a abc def");

//     constexpr auto tokens = res.get_tokens();

//     REQUIRE(tokens.size() == 3);
//     REQUIRE(tokens[0] == "a");
//     REQUIRE(tokens[1] == "abc");
//     REQUIRE(tokens[2] == "def");
// }