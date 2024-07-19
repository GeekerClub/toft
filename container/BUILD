cc_library(
    visibility = ['PUBLIC'],
    name = 'container',
    deps = [
        ':bitmap',
        ':bloom_filter',
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'bitmap',
    srcs = 'bitmap.cpp',
    deps = [
        '//thirdparty/toft/base/string:string',
    ]
)

cc_test(
    name = 'bitmap_test',
    srcs = 'bitmap_test.cpp',
    deps = ':bitmap',
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'bloom_filter',
    srcs = 'bloom_filter.cpp',
    deps = [
        '//thirdparty/toft/base/string:string',
        '//thirdparty/toft/base/:uint32_divisor',
        '//thirdparty/toft/hash:hash',
    ]
)

cc_test(
    name = 'bloom_filter_test',
    srcs = [
        'bloom_filter_test.cpp'
    ],
    deps = [
        ':bloom_filter',
        '//thirdparty/toft/system/time:time'
    ]
)

cc_test(
    name = 'skiplist_test',
    srcs = 'skiplist_test.cpp',
    deps = [
        '//thirdparty/toft/base:arena',
        '//thirdparty/toft/base:random',
        '//thirdparty/toft/hash:hash',
    ]
)

cc_test(
    name = 'lru_cache_test',
    srcs = ['lru_cache_test.cpp'],
    deps = [
        '//thirdparty/toft/system/threading:threading',
        '//thirdparty/toft/system/time:time',
    ],
)

cc_test(
    name = 'blocking_queue_test',
    srcs = [
        'blocking_queue_test.cpp',
    ],
    deps = [
        '//thirdparty/toft/system/threading:_sync_object',
        '//thirdparty/toft/system/threading:_thread',
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'counter',
    srcs = [ 
        'counter.cc'
    ],  
    deps = [ 
        '//thirdparty/toft/system/threading:threading',
        '//thirdparty/toft/system/timer:timer'
    ]   
)

cc_test(
    name = 'counter_test',
    srcs = 'counter_test.cc',
    deps = [ 
        ':counter',
        '#gtest',
        '#gflags',
        '#glog'
    ]   
)
