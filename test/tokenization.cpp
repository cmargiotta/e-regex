#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "e_regex.hpp"
#include "tokenizer.hpp"

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

TEST_CASE("Tokenization at compile time")
{
    using t = e_regex::token_t<"[^\\s]+", "a abc def", "\\s">;

    constexpr auto tokens = t::tokens::tokens;

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == "abc");
    REQUIRE(tokens[2] == "def");
}

TEST_CASE("Classified tokenization")
{
    enum class type
    {
        WORD,
        NUMBER
    };

    constexpr auto tokenizer = e_regex::tokenize<"(\\w+)|(\\d+)", "\\s", type>;

    constexpr auto res = tokenizer("a abc 123");

    std::vector<decltype(*res.begin())> tokens;

    for (auto token: res)
    {
        tokens.push_back(token);
    }

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].string == "a");
    REQUIRE(tokens[0].type == type::WORD);
    REQUIRE(tokens[1].string == "abc");
    REQUIRE(tokens[1].type == type::WORD);
    REQUIRE(tokens[2].string == "123");
    REQUIRE(tokens[2].type == type::NUMBER);
}

TEST_CASE("Constexpr classified tokenization")
{
    enum class type
    {
        WORD,
        NUMBER
    };

    using t = e_regex::token_t<"(\\w+)|(\\d+)", "a abc 123", "\\s", type>;

    constexpr auto tokens = t::tokens::tokens;

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].string == "a");
    REQUIRE(tokens[0].type == type::WORD);
    REQUIRE(tokens[1].string == "abc");
    REQUIRE(tokens[1].type == type::WORD);
    REQUIRE(tokens[2].string == "123");
    REQUIRE(tokens[2].type == type::NUMBER);
}