cc_library(
    name = 'timer',
    srcs = 'timer_manager.cpp',
    deps = [
        '//toft/system/threading:threading',
    ]
)

cc_test(
    name = 'timer_manager_test',
    srcs = 'timer_manager_test.cpp',
    deps = [
        ':timer',
    ]
)
