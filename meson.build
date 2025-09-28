project(
  'simple-json-parser',
  'cpp',
  version : '0.1',
  default_options : [
    'cpp_std=c++20',
    'warning_level=3',  # similar to -Wall -Wextra
  ]
)

# Add stricter warnings (like your -Wpedantic etc.)
add_project_arguments(
  '-Wpedantic', '-Wconversion', '-Wswitch',
  language : 'cpp'
)

# The library
sjp_lib = static_library(
  'sjp',
  ['src/parser.cpp', 'src/tokenizer.cpp'],
  include_directories : include_directories('include'),
  install : true
)

# Main executable (only built if this is the main project)
if meson.is_subproject() == false
  main_exe = executable(
    'main',
    'src/main.cpp',
    link_with : sjp_lib,
    include_directories : include_directories('include')
  )

  # Enable testing
  test_deps = [
    dependency('gtest', required : false),
    dependency('gtest_main', required : false),
  ]

  if test_deps[0].found()
    test_exe = executable(
      'test_parser',
      ['test/parser_test.cpp'],
      link_with : sjp_lib,
      dependencies : test_deps,
      include_directories : include_directories('include')
    )
    test('parser tests', test_exe)
  endif
endif

