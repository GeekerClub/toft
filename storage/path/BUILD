cc_library(
    visibility = ['PUBLIC'],
    name = 'path',
    srcs = [
        'path.cpp',
        'path_ext.cc'
    ],
    deps = '//thirdparty/toft/base/string:string',
)


cc_test(
    name = 'path_test',
    srcs = ['path_test.cpp'],
    deps = [':path']
)

