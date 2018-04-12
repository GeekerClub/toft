// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: An Qin (anqin.qin@gmail.com)

#include "toft/compress/block/quicklz.h"
#include <string>
using namespace std;

namespace toft {
QuicklzCompression::QuicklzCompression() :
    m_qlz_compress_state(NULL),
    m_qlz_decompress_state(NULL),
    m_compress_buffer_size(MAX_COMPRESSED_SIZE),
    m_uncompress_buffer_size(MAX_UNCOMPRESSED_SIZE) {
}

QuicklzCompression::~QuicklzCompression() {
    free(m_qlz_compress_state);
    m_qlz_compress_state = NULL;

    free(m_qlz_decompress_state);
    m_qlz_decompress_state = NULL;
}

bool QuicklzCompression::DoCompress(const char* str, size_t length, string* output) {
    if (length > MAX_CAPACITY) {
        return false;
    }

    if (!CheckCompressState()) {
        return false;
    }

    if (m_compress_buffer_size < length + OVERLOAD_SIZE) {
        m_compress_buffer_size = length + OVERLOAD_SIZE;
        m_compress_buffer.reset(new char[m_compress_buffer_size]);
    }
    size_t compressed_size = qlz_compress(
            str, m_compress_buffer.get(),
            length, m_qlz_compress_state);
    output->assign(m_compress_buffer.get(), compressed_size);
    return true;
}

bool QuicklzCompression::DoUncompress(const char* str, size_t length, string* output) {
    if (length > MAX_CAPACITY) {
        return false;
    }

    if (!CheckUncompressState()) {
        return false;
    }

    if (m_uncompress_buffer_size < length * 3) {
        m_uncompress_buffer_size = length * 3;
        m_uncompress_buffer.reset(new char[m_uncompress_buffer_size]);
    }

    size_t uncompression_size = qlz_decompress(
            str, m_uncompress_buffer.get(), m_qlz_decompress_state);
    output->assign(m_uncompress_buffer.get(), uncompression_size);
    return true;
}
TOFT_REGISTER_BLOCK_COMPRESSION(QuicklzCompression, "quicklz");
} // namespace toft
