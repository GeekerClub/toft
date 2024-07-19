cc_library(
    visibility = ['PUBLIC'],
    name = 'random',
    srcs = [
        'pseudo_random.cpp',
        'true_random.cpp',
    ]
)

cc_test(
    name = 'pseudo_random_test',
    srcs = 'pseudo_random_test.cpp',
    deps = ':random'
)

cc_test(
    name = 'true_random_test',
    srcs = 'true_random_test.cpp',
    deps = ':random'
)


