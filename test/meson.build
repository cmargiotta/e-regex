if not meson.is_subproject()
        tokenization_test = executable(
                'tokenization',
                'tokenization.cpp',
                dependencies: [eregex, catch2]
        )

        regex_test = executable(
                'regex',
                'regex.cpp',
                dependencies: [eregex, catch2]
        )

        strings_test = executable(
                'strings',
                'strings.cpp',
                dependencies: [eregex, catch2]
        )

        heuristics_test = executable(
                'heuristics',
                'heuristics.cpp',
                dependencies: [eregex, catch2]
        )

        test('strings test', strings_test)
        test('regex test', regex_test)
        test('tokenization test', tokenization_test)
        test('heuristics test', heuristics_test)
endif