// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#ifndef TOFT_COMPRESS_BLOCK_STREAM_COMPRESSION_H
#define TOFT_COMPRESS_BLOCK_STREAM_COMPRESSION_H

#include <stdint.h>
#include <string>

#include "toft/base/scoped_ptr.h"
#include "toft/system/threading/mutex.h"
#include "toft/compress/block/block_compression.h"

namespace toft {

class StreamCompression {
public:
    StreamCompression(const std::string& name);
    ~StreamCompression();

    bool AddBuffer(const char* buffer, size_t size);
    bool AddBuffer(const std::string& buffer);

    bool Compress(std::string* output);
    bool Uncompress(std::string* output);

    std::string GetName() {
        return m_compression->GetName();
    }

    std::string ErrorString() {
        return m_err;
    }

    size_t GetBufferSize() {
        return m_buffer.size();
    }

private:
    bool UncompressPrefix(const char* buffer, size_t size,
                          std::string* output, size_t *consumed_len);
    void PutFixed32(uint32_t value, std::string* dst);
    void GetFixed32(const char* pos, uint32_t* value);

private:
    Mutex m_mutex;

    scoped_ptr<BlockCompression> m_compression;
    std::string m_buffer;

    std::string m_err;
};


} // namespace toft

#endif  // TOFT_COMPRESS_BLOCK_STREAM_COMPRESSION_H

