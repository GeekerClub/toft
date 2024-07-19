cc_library(
    visibility = ['PUBLIC'],
    name = 'recordio',
    srcs = [
        'recordio.cc',
        'reverse_recordio.cc'
    ],
    deps = [
        '//thirdparty/toft/storage/file:file',
        '//thirdparty/protobuf:protobuf'
    ]
)

proto_library(
    name = 'document_proto',
    srcs = 'document.proto'
)

cc_test(
    name = 'recordio_test',
    srcs = 'recordio_test.cc',
    deps = [
        ':recordio',
        ':document_proto'
    ],
    testdata = [
        'document.proto'
    ]
)

cc_test(
    name = 'reverse_recordio_test',
    srcs = 'reverse_recordio_test.cc',
    deps = [
        ':recordio',
        ':document_proto'
    ],
    testdata = [
        'document.proto'
    ]
)
