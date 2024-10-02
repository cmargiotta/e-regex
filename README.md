# E(mpowered)-regex

![Alla pugna!](https://img.shields.io/badge/ALLA-PUGNA-F70808?style=for-the-badge)

Fast regular expression library, with full matching support, even at compile time!

## Regex matching

```cpp
constexpr e_regex::regex<"a(a*)"> matcher;

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
The regex expression will be automatically optimized using a built-in heuristics engine.

### Decomposition

An handy way of accessing multiple groups in a match is decomposition:

```cpp
constexpr e_regex::match<R"((\d+)-(\d+)-(\d+))"> matcher;

auto [match, year, month, day] = matcher("1970-01-01");
```

The four variables will be `std::string_view` and `match` will be `"1970-01-01"`, `year` will be `"1970"` and so on.

When a match is not found, this will only produce empty strings.

The number of variables in the decomposition must be **exactly** the number of groups in the regex, otherwise a static assertion will fail. If the regex contains `|` operators, then the number of variables must be the sum of groups in the branches.

### Inspection

Regex matchers provide methods to inspect their properties.

Given a regex `matcher`:

``` cpp
matcher.get_expression()
```

Will return the expression derived for the regex tree. This expression will have every optimization applied, so:
`\d+\s+123` will become `[0-9]++\s++123`.

``` cpp
matcher.groups
```

Will be the number of groups detected in the regex.

### Heuristics

The regex engine will automatically apply optimization heuristics recursively to an instanced regex.
Those heuristics will not alter the matched set of the regex, but can heavily affect matching performance.

| **Rule**                | **Conditions**                                                                         | **Result**                                                                                            |
|:------------------------|:---------------------------------------------------------------------------------------|:------------------------------------------------------------------------------------------------------|
| Greedy removal          | A greedy node's children set of accepted strings does not present intersection with it | The greedy node becomes a possessive node, removing backtracking logics (e.g. `\d+A` becomes `\d++A`) |
| Lazy removal            | A greedy node's children set of accepted strings does not present intersection with it | The lazy node becomes a possessive node (e.g. `\d+?A` becomes `\d++A`)                                |
| Empty node substitution | An empty node has only one child                                                       | The node becomes its child                                                                            |

### Operators

| **Identifier**  | **Description**                                                                                         |
|:----------------|:--------------------------------------------------------------------------------------------------------|
| `[delim]`       | Matches one of characters described by `delim`: a sequence of characters or a range in form `x-y`       |
| `[^delim]`      | Negation of `[delim]`                                                                                   |
| `(delim)`       | Identifies a group, `delim` can be any valid regex                                                      |
| `(?:delim)`     | Identifies an anonymous group: it does not produce a group in parsing, but delimits content             |
| `+`, `++`, `+?` | Respectively greedy, possessive and lazy at-least-one operator                                          |
| `*`, `*+`, `*?` | Respectively greedy, possessive and lazy Kleene star operator                                           |
| `?`, `?+`, `??` | Respectively greedy, possessive and lazy optional operator                                              |
| `{n}`           | Accepts only `n` repetitions of the previous identifier                                                 |
| `{n,}`          | Accepts at least `n` repetitions of the previous identifier                                             |
| `{n,m}`         | Accepts at least `n` and at most `m` repetitions of the previous identifier                             |
| `$`             | Matches end of line or end of input                                                                     |
| `^`             | Matches start of line or start of input                                                                 |
| `\xN`           | Matches hex data: N can be an hex nibble, two hex nibbles or `{N...}`, a brace enclosed nibble sequence |
| `\oN`           | Matches octal data: N can be three octal nibbles or `{N...}`, a brace enclosed nibble sequence          |

Operators can be escaped with `\` to match their character.

### Character classes

| **Identifier** | **Description**                               |
|:---------------|:----------------------------------------------|
| `\s`           | Space characters: '\t', '\n', '\f', '\r', ' ' |
| `\d`           | Digits: `[0-9]`                               |
| `\D`           | Not digit: `[^0-9]`                           |
| `\w`           | Word character: `[A-Za-z0-9_]`                |
| `\W`           | Not word character: `[^A-Za-z0-9_]`           |
| `\a`           | Alarm: `\x07`                                 |
| `.`            | Any character                                 |

## Tokenization

This library provides a regex-based tokenization engine, that can be used to easily build tokenizers.

`e_regex::tokenization::tokenizer<auto... definitions>` accepts a sequence of `e_regex::separator` and `e_regex::token` instances.
Every `e_regex::token` instance will describe a token matcher with a `type`: every instance must use the same enum for this field!
`e_regex::separator` instances will describe separators, aka matchers that will not produce tokens.

A tokenizer will stop parsing if:
- input string is fully parsed;
- input string is not accepted by any token matcher or separator matcher;
- after a token, a separator is not found.

Tokenization is `constexpr` flagged: if the input is `constexpr`, the result can be iterated at compile time.

Invoking a tokenizer with a `std::string_view` will produce a `e_regex::tokenization::iterator`: this iterator can be used in a range-based for loop or can be manually iterated using the increment operator.

```cpp
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

auto res = tokenizer("a 123 def");

for (auto token: res)
{
    ...
}
```

In this example, `tokenizer` will tokenize words and numbers separated by spaces. The `token` in the range loop will be `{.type = type::WORD, .value = "a"}`, `{.type = type::NUMBER, .value = "abc"}` and `{.type = type::WORD, .value = "def"}`.

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

## Performance

Benchmarks from the excellent (work)[https://github.com/mariomka/regex-benchmark] of mariomka have been ran on a Ryzen 7 7700X at 5.57GHz; the results are:

Language | Email(ms) | URI(ms) | IP(ms) | Total(ms)
--- | ---: | ---: | ---: | ---:
**Nim Regex** | 0.51 | 16.48 | 3.83 | 20.83
**C++ e-regex** | 11.79 | 10.87 | 4.13 | 26.80
**PHP** | 11.63 | 12.52 | 2.72 | 26.88
**Rust** | 16.32 | 14.22 | 2.07 | 32.61
**Crystal** | 25.21 | 24.19 | 6.21 | 55.61
**C++ Boost** | 26.08 | 26.45 | 10.37 | 62.90
**Julia** | 53.88 | 46.97 | 2.66 | 103.50
**Perl** | 58.59 | 39.53 | 13.02 | 111.14
**Dart** | 48.69 | 51.10 | 42.23 | 142.02
**C PCRE2** | 77.36 | 70.20 | 8.30 | 155.85
**D dmd** | 118.73 | 122.59 | 3.71 | 245.04
**D ldc** | 126.13 | 122.90 | 3.48 | 252.52
**Python PyPy2** | 85.05 | 79.99 | 114.31 | 279.35
**Ruby** | 154.53 | 135.38 | 29.70 | 319.62
**Kotlin** | 92.76 | 115.61 | 137.44 | 345.81
**Python PyPy3** | 139.41 | 126.26 | 119.58 | 385.24
**Dart Native** | 195.81 | 229.57 | 4.91 | 430.29
**Java** | 157.80 | 143.49 | 166.36 | 467.65
**Go** | 153.04 | 150.41 | 243.94 | 547.39
**C++ STL** | 233.24 | 190.29 | 154.17 | 577.70
**C# Mono** | 1527.89 | 1282.36 | 74.80 | 2885.05

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
