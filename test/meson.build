test_main = static_library('test_main', sources: ['catch_main.cpp'], dependencies: [catch2])

tokenization_test = executable(
        'tokenization',
        'tokenization.cpp',
        link_with: [test_main],
        dependencies: [eregex, catch2]
)

regex_test = executable(
        'regex',
        'regex.cpp',
        link_with: [test_main],
        dependencies: [eregex, catch2]
)

strings_test = executable(
        'strings',
        'strings.cpp',
        link_with: [test_main],
        dependencies: [eregex, catch2]
)

test('strings test', strings_test)
test('regex test', regex_test)
test('tokenization test', tokenization_test)