# Copyright 2012 Yunrang Inc. All Rights Reserved.
# Author: shunpingye@yunrang.com (Shunping Ye)
#
# Description:

cc_library(
    visibility = ['PUBLIC'],name = 'sharding',
           srcs = ['sharding.cc',
                   'fingerprint_sharding.cc',
                   ],
           deps = [
                   '//thirdparty/toft/base:class_registry',
                   '//thirdparty/toft/hash:hash',
                   ],
           link_all_symbols=True,
           )
