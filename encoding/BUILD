cc_library(
    name = 'encoding',
    srcs = [
        'ascii.cpp',
        'base64.cpp',
        'hex.cpp',
        'percent.cpp',
        'shell.cpp',
    ],
    deps = [
        '//toft/base/string:string',
        '//thirdparty/stringencoders:stringencoders'
    ]
)

cc_library(
    name = 'varint',
    srcs = 'varint.cpp',
    deps = [],
)

cc_library(
    name = 'charset_converter',
    srcs = 'charset_converter.cpp',
    deps = [
        '//toft/base/string:string',
    ]
)

cc_test(
    name = 'charset_converter_test',
    srcs = 'charset_converter_test.cpp',
    deps = [
        ':charset_converter'
    ],
)

cc_test(
    name = 'varint_test',
    srcs = 'varint_test.cpp',
    deps = [':varint'],
)

cc_test(
    name = 'ascii_test',
    srcs = 'ascii_test.cpp',
    deps = ':encoding'
)

cc_test(
    name = 'percent_test',
    srcs = 'percent_test.cpp',
    deps = ':encoding'
)

proto_library(
    name = 'unittest_proto',
    srcs = 'unittest.proto',
)

cc_library(
    name = 'proto_json_format',
    srcs = 'proto_json_format.cpp',
    deps = ['//thirdparty/glog:glog',
            '//thirdparty/jsoncpp:jsoncpp',
            '//thirdparty/protobuf:protobuf',
            '//toft/base/string:_number',
           ],
)

cc_test(
    name = 'proto_json_format_unittest',
    srcs = 'proto_json_format_unittest.cpp',
    deps = [':proto_json_format',
            ':unittest_proto',
            '//toft/storage/file:file',
           ],
    testdata = ['debug_string.txt',
                'json_fast_string.txt',
                'json_styled_string.txt',
               ]
)

cc_test(
    name = 'shell_test',
    srcs = 'shell_test.cpp',
    deps = ':encoding'
)

cc_test(
    name = 'hex_test',
    srcs = 'hex_test.cpp',
    deps = ':encoding',
)

cc_test(
    name = 'base64_test',
    srcs = 'base64_test.cpp',
    deps = ':encoding'
)

