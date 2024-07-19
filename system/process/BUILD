cc_library(
    visibility = ['PUBLIC'],
    name = 'process',
    srcs = [
        'process_local_const.cpp',
        'sub_process.cpp',
        'this_process.cpp',
    ],
    deps = [
        '//thirdparty/toft/encoding:encoding',
        '//thirdparty/toft/storage/file:file',
        '//thirdparty/toft/system/threading:_this_thread',
        '//thirdparty/toft/system/time:time',
    ]
)

cc_test(
    name = 'process_local_const_test',
    srcs = 'process_local_const_test.cpp',
    deps = ':process'
)

cc_test(
    name = 'sub_process_test',
    srcs = 'sub_process_test.cpp',
    deps = [
        ':process'
    ]
)

cc_test(
    name = 'this_process_test',
    srcs = 'this_process_test.cpp',
    deps = [
        ':process'
    ]
)
