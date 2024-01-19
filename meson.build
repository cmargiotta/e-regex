project('e-regex', 'cpp', default_options: ['warning_level=3', 'cpp_args=-std=c++23'])

if get_option('buildtype') == 'debug'
  add_project_arguments('-Og', '-g3',  language : 'cpp')
endif

subdir('src')
subdir('test')

quom = find_program('quom', required: false)

if quom.found()
	single_include = custom_target(
		output: 'single_include.hpp',
		command: [quom, '-I', meson.project_source_root() + '/src/', meson.project_source_root() + '/src/e_regex.hpp', '@OUTPUT@'],
		build_by_default: true
	)
endif