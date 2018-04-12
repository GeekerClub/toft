// Copyright (c) 2016, The Toft Authors.
// All rights reserved.
//
// Author: An Qin <anqin.qin@gmail.com>

#include "toft/compress/block/none.h"

namespace toft {

NoneCompression::NoneCompression() {}

NoneCompression::~NoneCompression() {}

bool NoneCompression::DoCompress(const char* str, size_t length, std::string* out) {
    out->assign(str, length);
    return true;
}

bool NoneCompression::DoUncompress(const char* str, size_t length, std::string* out) {
    out->assign(str, length);
    return true;
}

TOFT_REGISTER_BLOCK_COMPRESSION(NoneCompression, "none");

}  // namespace toft
