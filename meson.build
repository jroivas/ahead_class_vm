project('ahead_class_vm', 'cpp')

add_global_arguments('-std=c++11', language : 'cpp')
add_global_arguments('-fPIC', language : 'cpp')
add_global_arguments('-Wall', language : 'cpp')
add_global_arguments('-Wno-error=unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-unused-local-typedefs', language : 'cpp')
add_global_arguments('-Wno-error=pedantic', language : 'cpp')
add_global_arguments('-DGC_OPERATOR_NEW_ARRAY', language : 'cpp')

main_inc = include_directories('inc')
lib_inc = include_directories('lib')
top_inc = include_directories('.')

subdir('lib')
subdir('src')
subdir('test')
