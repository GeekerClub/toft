// Copyright (c) 2016, The Toft Authors.
// All rights reserved.
//
// Author: An Qin <anqin.qin@gmail.com>

#ifndef TOFT_COMPRESS_BLOCK_NONE_H
#define TOFT_COMPRESS_BLOCK_NONE_H

#include <string>

#include "toft/compress/block/block_compression.h"

namespace toft {

class NoneCompression : public BlockCompression {
    TOFT_DECLARE_UNCOPYABLE(NoneCompression);

public:
    NoneCompression();
    virtual ~NoneCompression();

    virtual std::string GetName() {
        return "none";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);
};

}  // namespace toft
#endif  // TOFT_COMPRESS_BLOCK_NONE_H
