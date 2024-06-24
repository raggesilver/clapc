project('clapc', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=gnu23'])

add_project_arguments('-Wno-gnu-statement-expression', language : 'c')
add_project_arguments('-Wno-pedantic', language : 'c')

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_CLAPC']

shlib = shared_library('clapc', 'clapc.c',
  install : true,
  c_args : lib_args,
  gnu_symbol_visibility : 'hidden',
)

test_exe = executable('clapc_test', 'clapc_test.c',
  link_with : shlib)
test('clapc', test_exe)

# Make this library usable as a Meson subproject.
clapc_dep = declare_dependency(
  include_directories: include_directories('.'),
  link_with : shlib)

# Make this library usable from the system's
# package manager.
install_headers('clapc.h', subdir : 'clapc')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
  name : 'clapc',
  filebase : 'clapc',
  description : 'Meson sample project.',
  subdirs : 'clapc',
  libraries : shlib,
  version : '0.1',
)
