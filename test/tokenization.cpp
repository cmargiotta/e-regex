#include <catch2/catch_test_macros.hpp>

#include <vector>

#include "e_regex.hpp"
#include "tokenization/tokenizer.hpp"

TEST_CASE("Classified tokenization")
{
    using e_regex::separator;
    using e_regex::token;

    enum class type
    {
        WORD,
        NUMBER
    };

    constexpr auto tokenizer
        = e_regex::tokenization::tokenizer<token {type::WORD, "[a-z]+"},
                                           token {type::NUMBER, "\\d+"},
                                           separator {"\\s"}> {};

    auto res = tokenizer("a abc 123");

    std::vector<decltype(*res)> tokens;

    for (auto token: res)
    {
        tokens.push_back(token);
    }

    REQUIRE(tokens.size() == 3);
    REQUIRE(tokens[0].value == "a");
    REQUIRE(tokens[0].type == type::WORD);
    REQUIRE(tokens[1].value == "abc");
    REQUIRE(tokens[1].type == type::WORD);
    REQUIRE(tokens[2].value == "123");
    REQUIRE(tokens[2].type == type::NUMBER);
}
