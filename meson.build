project('useful', 'c', license : 'LGPL2.1', version : '0.3',
        default_options : ['c_std=c11'])

# Update these for new releases if necessary
dir_name = meson.project_name()
lib_name = 'lib' + dir_name
lib_version = meson.project_version() + '.1'
project_description = 'Useful C data structures, algorithms and macros'


subdir('src')
subdir('tests')
subdir('doc')
