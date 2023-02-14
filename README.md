# E(mpowered)-regex

![Alla pugna!](https://img.shields.io/badge/ALLA-PUGNA-F70808?style=for-the-badge)

Extremely fast regular expression library, with full matching support, even at compile time!

## Basic usage

```cpp
constexpr auto matcher = e_regex::match<"a(a*)">;

auto result = matcher("zaaa1");
result.to_view(); //"aaa"
result[1]; //group 1: "aa"
```

Invoking the matcher with an `std::string_view` returns an `e_regex::match_result`.

`result[n]` and `result.get_group(n)` can be used to read the `n`th group. When `n` is 0, the whole match is returned.

`result.to_view()` and `static_cast<std::string_view>(result)` are identical to `result[0]`.

`result.size()` provides the number of groups identified.

`result.is_accepted()` and `static_cast<bool>(result)` evaluate to `true` only if the query is accepted by the regex.

`result.next()` computes the next match in the query and evalutes to `true` only if it is found.

Every function is `constexpr` and `noexcept`: if the query is constexpr too, no runtime will be involved!

### Decomposition

An handy way of accessing multiple groups in a match is decomposition:

```cpp
constexpr auto matcher = e_regex::match<R"((\d+)-(\d+)-(\d+))">;

auto [match, year, month, day] = matcher("1970-01-01");
```

The four variables will be `std::string_view` and `match` will be `"1970-01-01"`, `year` will be `"1970"` and so on.

When a match is not found, this will only produce empty strings.

The number of variables in the decomposition must be **exactly** the number of groups in the regex, otherwise a static assertion will fail. If the regex contains `|` operators, then the number of variables must be the maximum number of groups in the branches.

### Lambdas

`e_regex::match<...>` is simply a lambda expression.

```cpp
constexpr auto matcher = e_regex::match<"a(a*)">;
auto result = matcher("zaaa1");

auto result = e_regex::match<"a(a*)">("zaaa1");
```

Both are totally identical.

### Tokenization

Regexes with different branches (at least one) can be used to easily build tokenizers.

`e_regex::tokenize<token_regex, separator_regex>` will produce a `constexpr` functor capable of tokenizing a string; `token_regex` is the regex that matches tokens, `separator_regex` is the **optional** regex that matches separators, if it is not provided it is assumed that tokens are not separated.

```cpp
constexpr auto tokenizer = e_regex::tokenize<"\\s+|\\d+", "\\s">;

auto res = tokenizer("a abc def");

for (auto token: res)
{
    ...
}
```

In this example, `tokenizer` will tokenize words and numbers separated by spaces. The `token` in the range loop will be `"a"`, `"abc"` and `"def"`.

In a `consteval` contest it is possible to prebuild a token array using:

```cpp
using t = e_regex::token_t<"\\s+|\\d+", "a abc def", "\\s">;

constexpr auto tokens = t::tokens::tokens;
```

`tokens` will be a prebuilt `std::array` of `string_view` of size `3`.

`t::tokens` is a `std::integer_sequence`-like type of tokens, useful in template-heavy code.

#### **Typed tokenization**

It is possible to define an `enum class` with categories of tokens. This syntax requires a regex where every branch contains **exactly** one group and an enum with only consecutive values, starting from 0, and at least the same number of values and groups in the regex.

```cpp
enum class type
{
    WORD,
    NUMBER
};

constexpr auto tokenizer = e_regex::tokenize<"(\\w+)|(\\d+)", "\\s", type>;

constexpr auto res = tokenizer("a abc 123");

// Or, for prebuilding
using t = e_regex::token_t<"(\\w+)|(\\d+)", "a abc 123", "\\s", type>;
```

Here every generated token will a structure containing tha matching type:

```cpp
t::tokens::tokens[0].type == type::WORD;
t::tokens::tokens[0].string == "a";

t::tokens::tokens[2].type == type::NUMBER;
t::tokens::tokens[2].string == "123";
```


## Building and testing

This project is based on `meson`, `ninja` and `C++20`. If `quom` is available, a single include header will be automatically built by meson.

To build and run unit tests:
```bash
$ meson setup build
$ meson test -C build
```

The generated single include header will be placed in `build/single_include.hpp`.

It is included a minimal build environment defined in Nix, running
```bash
$ nix develop
```

starts a 1:1 replica of a working environment for this library with two simple aliases:

```
build     - Automatically configure build folder and run build
run_tests - Run tests
```
