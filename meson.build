project('e-regex', 'cpp', default_options: ['warning_level=3', 'cpp_args=-std=c++20'])

catch2 = dependency('catch2')

subdir('src')
subdir('test')

quom = run_command('quom', '-I ./src/', './src/e_regex.hpp', 'build/single_include.hpp', capture: true)

if quom.returncode() != 0
    warning('Cannot build the single include header:')
    warning(quom.stderr())
endif