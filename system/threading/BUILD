cc_library(
    visibility = ['PUBLIC'],
    name = '_sync_object',
    srcs = [
        'condition_variable.cpp',
        'event.cpp',
        'mutex.cpp',
        'semaphore.cpp',
        'rwlock.cpp',
        'spinlock.cpp',
    ],
    deps = [
        ':_this_thread',
        '//thirdparty/toft/system:check_error',
        '//thirdparty/toft/system/time:time',
        '#pthread'
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = '_this_thread',
    srcs = 'this_thread.cpp',
    deps = '#pthread',
)

cc_library(
    visibility = ['PUBLIC'],
    name = '_thread',
    srcs = [
        'thread.cpp',
        'base_thread.cpp',
    ],
    deps = [
        ':_this_thread',
        '//thirdparty/toft/system:check_error',
        '#pthread',
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = '_thread_group',
    srcs = [
        'thread_group.cpp',
    ],
    deps = [
        ':_thread',
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = '_thread_pool',
    srcs = [
        'thread_pool.cpp'
    ],
    deps = [
        ':_sync_object',
        ':_this_thread',
        ':_thread',
        '//thirdparty/toft/base:closure',
        '//thirdparty/toft/system/atomic:atomic'
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'threading',
    deps = [
        ':_sync_object',
        ':_this_thread',
        ':_thread',
        ':_thread_group',
        ':_thread_pool'
    ]
)

cc_test(
    name = 'event_test',
    srcs = 'event_test.cpp',
    deps = ':_sync_object',
)

cc_test(
    name = 'mutex_test',
    srcs = 'mutex_test.cpp',
    deps = ':_sync_object',
)

cc_test(
    name = 'rwlock_test',
    srcs = 'rwlock_test.cpp',
    deps = [
        ':_sync_object',
        ':_thread_group',
    ]
)

cc_test(
    name = 'semaphore_test',
    srcs = 'semaphore_test.cpp',
    deps = ':_sync_object'
)

cc_test(
    name = 'spinlock_test',
    srcs = 'spinlock_test.cpp',
    deps = [
        ':_sync_object',
        ':_thread_group',
    ]
)

cc_test(
    name = 'this_thread_test',
    srcs = 'this_thread_test.cpp',
    deps = [
        ':_this_thread',
        ':_thread',
    ]
)

cc_test(
    name = 'thread_test',
    srcs = 'thread_test.cpp',
    deps = [
        ':_this_thread',
        ':_thread',
    ]
)

cc_test(
    name = 'thread_group_test',
    srcs = 'thread_group_test.cpp',
    deps = [
        ':_sync_object',
        ':_thread_group',
    ]
)

cc_test(
    name = 'thread_pool_test',
    srcs = 'thread_pool_test.cpp',
    deps = [
        ':threading',
    ]
)
