# Copyright 2012. All Rights Reserved.
# Author: shunping_ye@staff.easou.com (Shunping Ye)
#
# Description:
#   TODO(shunpingye): describe this package.

cc_library(
    name='_block_compression',
    srcs=['block_compression.cpp', ],
    deps=[
        '//toft/base/string:string',
    ],
)

cc_library(
    name='snappy',
    srcs=['snappy.cpp', ],
    deps=[
        ':_block_compression',
        '//toft/base:class_registry',
        '//thirdparty/snappy:snappy',
    ],
    link_all_symbols=True,
)

cc_library(
    name='lzo',
    srcs=['lzo.cpp', ],
    deps=[
        ':_block_compression',
        '//toft/base:class_registry',
        '//thirdparty/lzo:lzo',
    ],
    link_all_symbols=True,
)

#cc_library(
#    name='quicklz',
#    srcs=['quicklz.cpp', ],
#    deps=[
#        ':_block_compression',
#        '//toft/base:class_registry',
#        '//thirdparty/quicklz:quicklz',
#    ],
#    link_all_symbols=True,
#)

cc_library(
    name='gzip',
    srcs=['gzip.cpp', ],
    deps=[
        ':_block_compression',
        '//toft/base:class_registry',
        '#z',
    ],
    link_all_symbols=True,
)

cc_library(
    name = 'none',
    srcs = 'none.cpp',
    deps=[
        ':_block_compression',
        '//toft/base:class_registry',
    ],
    link_all_symbols=True,
)

cc_library(
    name='block',
    deps=[
        ':lzo',
        ':snappy',
#        ':quicklz',
        ':gzip',
        ':none'
    ],
)


cc_library(
    name = 'stream_compression',
    srcs = [
        'stream_compression.cc'
    ],
    deps = [
        ':block',
        '//toft/system/threading:threading'
    ]
)

cc_test(
    name='block_compression_test',
    srcs=[
        'block_compression_test.cpp',
    ],
    deps=[
        ':block',
    ],
)

cc_test(
    name = 'stream_compression_test',
    srcs = 'stream_compression_test.cc',
    deps = [
        ':stream_compression',
        '//toft/storage/file:file'
    ]
)
