ahead_class_vm = executable('ahead_class_vm',
    sources: [
        'classloader.cpp',
        'vm_constants.cpp',
        'vm_methods.cpp',
        'vm_attributes.cpp',
        'vm_fields.cpp',
        'vm_exceptions.cpp',
        'vm_stack.cpp',
        'vm_object.cpp',
        'vm.cpp',
        'utils.cpp',
        'main.cpp'
    ],
    include_directories: [
        top_inc,
        main_inc
    ],
    link_with: [
        libclass
    ],
    link_args: '-lgc'
)
