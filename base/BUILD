cc_library(
    visibility = ['PUBLIC'],
    name = 'binary_version',
    srcs = [
        'binary_version.cpp',
        'setup_binary_version.cpp',
    ],
    deps = '#gflags',
)

cc_test(
    name = 'binary_version_test',
    srcs = 'binary_version_test.cpp',
    deps = [
        ':binary_version'
    ]
)

cc_library(
    visibility = ['PUBLIC'],name = 'byte_order', srcs = 'byte_order.cpp')

cc_library(
    visibility = ['PUBLIC'],
    name = 'class_registry',
    deps = './class_registry:_class_registry'
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'closure',
    srcs = 'closure.cpp',
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'benchmark',
    srcs = 'benchmark.cpp',
    deps = ['//thirdparty/toft/system/time:time',
            '#glog',
           # '//thirdparty/perftools:tcmalloc_and_profiler',
           ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'benchmark_main',
    srcs = 'benchmark_main.cpp',
    deps = [':benchmark',
            '#gflags',
           ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'int128',
    srcs = 'int128.cpp',
    deps = '#glog',
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'uint32_divisor',
    srcs = 'uint32_divisor.cpp'
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'random',
    srcs = 'random.cpp',
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'arena',
    srcs = 'arena.cpp',
)

cc_test(
    name = 'uint32_divisor_test',
    srcs = 'uint32_divisor_test.cpp',
    deps = ':uint32_divisor'
)

cc_test(
    name = 'arena_test',
    srcs = 'arena_test.cpp',
    deps = [
        ':arena',
        ':random',
    ]
)

cc_test(
    name = "object_pool_test",
    srcs = ["object_pool_test.cpp"],
    deps = [
        '//thirdparty/toft/system/threading:threading',
    ],
)
cc_test(name = 'array_size_test', srcs = 'array_size_test.cpp')
cc_test(name = 'byte_order_test', srcs = 'byte_order_test.cpp')
cc_test(name = 'closure_test', srcs = 'closure_test.cpp', deps = ':closure')
cc_test(name = 'functional_test', srcs = 'functional_test.cpp')
cc_test(name = 'intrusive_list_test', srcs = 'intrusive_list_test.cpp')
cc_test(name = 'preprocess_test', srcs = 'preprocess_test.cpp')
cc_test(name = 'shared_ptr_test', srcs = 'shared_ptr_test.cpp')
cc_test(name = 'singleton_test', srcs = 'singleton_test.cpp')
cc_test(name = 'static_assert_test', srcs = 'static_assert_test.cpp')
cc_test(name = 'static_resource_test', srcs = 'static_resource_test.cpp')
cc_test(name = 'stl_util_test', srcs = 'stl_util_test.cpp')
cc_test(name = 'unique_ptr_test', srcs = 'unique_ptr_test.cpp')
#cc_test(name = 'unordered_test', srcs = 'unordered_test.cpp')

