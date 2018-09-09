# Install docs


doxygen = find_program('doxygen', required : false)

if doxygen.found()
  run_target('gendocs',
             command : ['gendocs.sh',
                        meson.project_name(), meson.project_version()])

endif

install_subdir('html',  install_dir : join_paths(get_option('datadir'), 'doc',
                                                 'useful'))
install_subdir('examples',  install_dir : join_paths(get_option('datadir'), 'doc',
                                                     'useful'))
