// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: An Qin (anqin.qin@gmail.com)

#ifndef TOFT_COMPRESS_BLOCK_QUICKLZ_H
#define TOFT_COMPRESS_BLOCK_QUICKLZ_H

#include <stdint.h>
#include <string>
#include "thirdparty/quicklz/quicklz.h"
#include "toft/base/scoped_ptr.h"
#include "toft/compress/block/block_compression.h"

namespace toft {
class QuicklzCompression : public BlockCompression {
    TOFT_DECLARE_UNCOPYABLE(QuicklzCompression);

public:
    QuicklzCompression();
    virtual ~QuicklzCompression();

    virtual std::string GetName() {
        return "quicklz";
    }

private:
    virtual bool DoCompress(const char* str, size_t length, std::string* out);
    virtual bool DoUncompress(const char* str, size_t length, std::string* out);

    bool CheckCompressState() {
        if (m_qlz_compress_state == NULL) {
            m_qlz_compress_state = reinterpret_cast<qlz_state_compress*>
                (malloc(sizeof(qlz_state_compress)));
            memset(m_qlz_compress_state, 0, sizeof(qlz_state_compress));
        }
        if (m_compress_buffer.get() == NULL) {
            m_compress_buffer.reset(new char[m_compress_buffer_size]);
            if (m_compress_buffer.get() == NULL) {
                return false;
            }
        }
        return true;
    }

    bool CheckUncompressState() {
        if (m_qlz_decompress_state == NULL) {
            m_qlz_decompress_state = reinterpret_cast<qlz_state_decompress*>
                (malloc(sizeof(qlz_state_decompress)));
            memset(m_qlz_decompress_state, 0, sizeof(qlz_state_decompress));
        }
        if (m_uncompress_buffer.get() == NULL) {
            m_uncompress_buffer.reset(new char[m_uncompress_buffer_size]);
            if (m_uncompress_buffer.get() == NULL) {
                return false;
            }
        }
        return true;
    }

    qlz_state_compress* m_qlz_compress_state;
    qlz_state_decompress* m_qlz_decompress_state;
    scoped_array<char> m_compress_buffer;
    size_t m_compress_buffer_size;
    scoped_array<char> m_uncompress_buffer;
    size_t m_uncompress_buffer_size;

    static const size_t MAX_CAPACITY = 256 * 1024;
    static const size_t MAX_COMPRESSED_SIZE = 64 * 1024;
    static const size_t MAX_UNCOMPRESSED_SIZE = MAX_COMPRESSED_SIZE * 3;
    // compression header and other information may take some space
    static const size_t OVERLOAD_SIZE = 400;
};
} // namespace toft

#endif // TOFT_COMPRESS_BLOCK_QUICKLZ_H
