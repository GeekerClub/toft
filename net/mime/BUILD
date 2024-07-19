cc_library(
    visibility = ['PUBLIC'],
    name = 'mime',
    srcs = 'mime.cpp',
    deps = [
        '//thirdparty/toft/base/string:string',
        '#glog'
    ]
)

cc_test(
    name = 'mime_test',
    srcs = 'mime_test.cpp',
    deps = ':mime'
)
