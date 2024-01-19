project('e-regex', 'cpp', default_options: ['warning_level=3', 'cpp_args=-std=c++23'])

if get_option('buildtype') == 'debug'
  add_project_arguments('-Og', '-g3',  language : 'cpp')
endif

catch2 = dependency('catch2-with-main')

subdir('src')
subdir('test')

quom = find_program('quom', required: false)

if quom.found()
    quom_output = run_command(quom, '-I', meson.project_source_root() + '/src/', meson.project_source_root() + '/src/e_regex.hpp', meson.project_build_root() + '/single_include.hpp', capture: true)

    if quom_output.returncode() != 0
        warning('Cannot build the single include header:')
        warning(quom_output.stderr())
    endif
endif