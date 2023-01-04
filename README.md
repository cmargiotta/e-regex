# E(mpowered)-regex

Extremely fast regular expression library, with full matching support, even at compile time!

## Basic usage

```cpp
auto matcher = e_regex::match<"a(a*)">;

auto result = matcher("zaaa1");
result.groups(0); //"aaa"
result.groups(1); //"aa"
```

### TODO: details
