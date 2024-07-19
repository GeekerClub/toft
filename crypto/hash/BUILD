cc_library(
    visibility = ['PUBLIC'],
    name = 'md5',
    srcs = 'md5.cpp',
    deps = [
        '//thirdparty/toft/base:int128',
        '//thirdparty/toft/base/string:string',
        '//thirdparty/toft/encoding:encoding',
    ],
)

cc_test(
    name = 'md5_test',
    srcs = 'md5_test.cpp',
    deps = [':md5'],
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'sha1',
    srcs = 'sha1.cpp',
)

cc_test(
    name = 'sha1_test',
    srcs = 'sha1_test.cpp',
    deps = [
        ':sha1',
        '//thirdparty/toft/encoding:encoding',
    ],
)
