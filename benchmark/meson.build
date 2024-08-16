if not meson.is_subproject()
        bench = executable(
                'regex_benchmark',
                'main.cpp',
                dependencies: [eregex]
        )

        benchmark('regex benchmark', bench, args: [
		meson.source_root() / 'benchmark/input-text.txt'
	],
	timeout: 600)
endif
