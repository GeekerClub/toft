// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: An Qin (anqin.qin@gmail.com)

#ifndef TOFT_COMPRESS_BLOCK_GZIP_H
#define TOFT_COMPRESS_BLOCK_GZIP_H

#include <stdint.h>
#include <zlib.h>
#include <string>
#include "toft/base/scoped_ptr.h"
#include "toft/compress/block/block_compression.h"

namespace toft {
class GzipCompression : public BlockCompression {
    TOFT_DECLARE_UNCOPYABLE(GzipCompression);

public:
    GzipCompression();
    virtual ~GzipCompression();

    virtual std::string GetName() {
        return "gzip";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);

    bool CheckInflateStream();
    bool CheckDeflateStream();
    z_stream* m_zlib_deflate_stream;
    z_stream* m_zlib_inflate_stream;
    scoped_array<char> m_deflat_buffer;
    scoped_array<char> m_inflat_buffer;
    size_t m_deflat_buffer_size;
    size_t m_inflat_buffer_size;

    static const int BAIDU_HADOOP_ZLIB_MEM_LEVEL = 8;
    // gzip_format-31, default-15
    static const size_t BAIDU_HADOOP_WINDOW_SIZE = 31;
    static const size_t MAX_BUFFER_SIZE = 256 * 1024;
    // default-(-1), best_speed-1, best_compression-9
    static const int DEFAULT_ZIP_LEVEL = 1;
    // default-0, filtered-1, huffman-2, rle-3, fixed-4
    static const int DEFAULT_STRATEGY = 0;
};
} // namespace toft

#endif // TOFT_COMPRESS_BLOCK_GZIP_H
