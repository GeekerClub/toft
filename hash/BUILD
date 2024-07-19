cc_library(
    visibility = ['PUBLIC'],
    name = 'hash',
    deps = [
        ':city',
        ':crc32',
        ':jenkins',
        ':murmur',
        ':fingerprint',
        ':super_fast',
    ],
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'city',
    srcs = 'city.cpp',
    deps = [
        '//thirdparty/toft/base:int128',
        '//thirdparty/cityhash:cityhash',
    ]
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'jenkins',
    srcs = 'jenkins.cpp',
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'fingerprint',
    srcs = [
        'fingerprint32.cpp',
        'fingerprint64.cpp',
    ],
    deps = [
        ':murmur',
        '//thirdparty/stringencoders:stringencoders'
    ],
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'super_fast',
    srcs = 'super_fast.cpp',
)


cc_library(
    visibility = ['PUBLIC'],
    name = 'murmur',
    srcs = [
        'murmur/MurmurHash2_64.cpp',
        'murmur/MurmurHash2A.cpp',
        'murmur/MurmurHash2.cpp',
        'murmur/MurmurHash3.cpp',
        'murmur/MurmurHashAligned2.cpp',
        'murmur/MurmurHashNeutral2.cpp',
    ],
)

cc_test(
    name = 'hash_test',
    srcs = 'hash_test.cpp',
    deps = [':hash'],
)

cc_library(
    visibility = ['PUBLIC'],
    name = 'crc32',
    srcs = 'crc32.cpp',
    deps = [
        '//thirdparty/toft/encoding:encoding',
    ],
)

cc_test(
    name = 'crc32_test',
    srcs = 'crc32_test.cpp',
    deps = [
        ':crc32',
    ],
)

#cc_benchmark(
#    name = 'hash_benchmark',
#    srcs = 'hash_benchmark.cpp',
#    deps = [':hash'],
#)
