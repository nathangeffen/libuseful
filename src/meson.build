cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
src = ['algorithms.c',  'array.c',  'csv.c',  'test.c', 'string.c']

lib = shared_library(meson.project_name(), sources : src, dependencies: m_dep,
                     version : lib_version, soversion : '0', install : true)


include_subdir = join_paths(dir_name, 'useful')

install_headers('useful.h', subdir: dir_name)
install_headers('useful/array.h', 'useful/algorithms.h', 'useful/check.h',
                'useful/csv.h', 'useful/test.h', 'useful/string.h',
                subdir: include_subdir)

# Pkgconfig
pkg = import('pkgconfig')
libs = '-l' + dir_name
h = [dir_name, ]
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : meson.project_name(),
             filebase : dir_name,
             description : project_description)
