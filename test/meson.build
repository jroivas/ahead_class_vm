test_vm = executable('test_class_vm',
    sources: [
        'test.cpp',
        'test_stack.cpp',
    ],
    include_directories: [
        top_inc,
        main_inc
    ],
    link_with: [
        libclass
    ],
    link_args: [
         '-lgc',
         '-lgccpp'
    ]
)

test('main_test', test_vm)
