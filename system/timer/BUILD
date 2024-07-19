cc_library(
    visibility = ['PUBLIC'],
    name = 'timer',
    srcs = 'timer_manager.cpp',
    deps = [
        '//thirdparty/toft/system/threading:threading',
    ]
)

cc_test(
    name = 'timer_manager_test',
    srcs = 'timer_manager_test.cpp',
    deps = [
        ':timer',
    ]
)
