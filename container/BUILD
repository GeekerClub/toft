cc_library(
    name = 'container',
    deps = [
        ':bitmap',
        ':bloom_filter',
    ]
)

cc_library(
    name = 'bitmap',
    srcs = 'bitmap.cpp',
    deps = [
        '//toft/base/string:string',
    ]
)

cc_test(
    name = 'bitmap_test',
    srcs = 'bitmap_test.cpp',
    deps = ':bitmap',
)

cc_library(
    name = 'bloom_filter',
    srcs = 'bloom_filter.cpp',
    deps = [
        '//toft/base/string:string',
        '//toft/base/:uint32_divisor',
        '//toft/hash:hash',
    ]
)

cc_test(
    name = 'bloom_filter_test',
    srcs = [
        'bloom_filter_test.cpp'
    ],
    deps = [
        ':bloom_filter',
        '//toft/system/time:time'
    ]
)

cc_test(
    name = 'skiplist_test',
    srcs = 'skiplist_test.cpp',
    deps = [
        '//toft/base:arena',
        '//toft/base:random',
        '//toft/hash:hash',
    ]
)

cc_test(
    name = 'lru_cache_test',
    srcs = ['lru_cache_test.cpp'],
    deps = [
        '//toft/system/threading:threading',
        '//toft/system/time:time',
    ],
)

cc_test(
    name = 'blocking_queue_test',
    srcs = [
        'blocking_queue_test.cpp',
    ],
    deps = [
        '//toft/system/threading:_sync_object',
        '//toft/system/threading:_thread',
    ]
)

cc_library(
    name = 'counter',
    srcs = [ 
        'counter.cc'
    ],  
    deps = [ 
        '//toft/system/threading:threading',
        '//toft/system/timer:timer'
    ]   
)

cc_test(
    name = 'counter_test',
    srcs = 'counter_test.cc',
    deps = [ 
        ':counter',
        '//thirdparty/gtest:gtest',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog'
    ]   
)
