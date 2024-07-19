// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: An Qin (anqin.qin@gmail.com)

#include "toft/compress/block/gzip.h"
#include "glog/logging.h"
namespace toft {
GzipCompression::GzipCompression() :
    m_zlib_deflate_stream(NULL), m_zlib_inflate_stream(NULL),
    m_deflat_buffer_size(0), m_inflat_buffer_size(0) {
}

bool GzipCompression::CheckDeflateStream() {
    if (m_zlib_deflate_stream == NULL) {
        m_zlib_deflate_stream = reinterpret_cast<z_stream*>(malloc(sizeof(z_stream)));
        if (m_zlib_deflate_stream == NULL) {
            return false;
        }
        memset(m_zlib_deflate_stream, 0, sizeof(z_stream));
        int ret = deflateInit2_(m_zlib_deflate_stream,
                Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                BAIDU_HADOOP_WINDOW_SIZE,
                BAIDU_HADOOP_ZLIB_MEM_LEVEL,
                DEFAULT_STRATEGY,
                ZLIB_VERSION, sizeof(z_stream));
        if (ret != Z_OK) {
            free(m_zlib_deflate_stream);
            m_zlib_deflate_stream = NULL;
            VLOG(8) << "deflateInit2_ failed, return code:"
                << ret;
            return false;
        }
    }
    return true;
}

bool GzipCompression::CheckInflateStream() {
    if (m_zlib_inflate_stream == NULL) {
        m_zlib_inflate_stream = reinterpret_cast<z_stream*>(malloc(sizeof(z_stream)));
        if (m_zlib_inflate_stream == NULL) {
            return false;
        }
        memset(m_zlib_inflate_stream, 0, sizeof(z_stream));
        int ret = inflateInit2_(m_zlib_inflate_stream,
                BAIDU_HADOOP_WINDOW_SIZE, ZLIB_VERSION,
                sizeof(z_stream));
        if (ret != Z_OK) {
            free(m_zlib_inflate_stream);
            m_zlib_inflate_stream = NULL;
            VLOG(8) << "inflateInit2_ failed, return code:"
                << ret;
            return false;
        }
    }
    return true;
}

GzipCompression::~GzipCompression() {
    if (m_zlib_deflate_stream) {
        int ret = deflateEnd(m_zlib_deflate_stream);
        if (ret == Z_STREAM_ERROR) {
            VLOG(8) << "deflateEnd failed!";
        }
    }

    if (m_zlib_inflate_stream) {
        int ret = inflateEnd(m_zlib_inflate_stream);
        if (ret == Z_STREAM_ERROR) {
            VLOG(8) << "inflateEnd failed!";
        }
    }
    free(m_zlib_deflate_stream);
    m_zlib_deflate_stream = NULL;
    free(m_zlib_inflate_stream);
    m_zlib_inflate_stream = NULL;
}

bool GzipCompression::DoUncompress(const char* str, size_t length, std::string* out) {
    if (length == 0) {
        *out = "";
        return true;
    }
    if (length > MAX_BUFFER_SIZE) {
        return false;
    }
    if (!CheckInflateStream()) {
        return false;
    }

    if (m_inflat_buffer_size < length * 3) {
        m_inflat_buffer_size = length * 3;
        m_inflat_buffer.reset(new char[m_inflat_buffer_size]);
    }
    m_zlib_inflate_stream->next_in = reinterpret_cast<Bytef*>(const_cast<char*>(str));
    m_zlib_inflate_stream->next_out = reinterpret_cast<Bytef*>(m_inflat_buffer.get());
    m_zlib_inflate_stream->avail_in = length;
    m_zlib_inflate_stream->avail_out = m_inflat_buffer_size;
    int ret = inflate(m_zlib_inflate_stream, Z_FINISH);
    if (ret == Z_STREAM_END) {
        out->assign(m_inflat_buffer.get(), m_inflat_buffer_size - m_zlib_inflate_stream->avail_out);
        return true;
    } else {
        LOG(ERROR)<< "fail to Uncompress data!" << ret;
        return false;
    }
}

bool GzipCompression::DoCompress(const char* str, size_t length, std::string* out) {
    if (length == 0) {
        *out = "";
        return true;
    }
    if (length > MAX_BUFFER_SIZE) {
        return false;
    }
    if (!CheckDeflateStream()) {
        return false;
    }

    if (m_deflat_buffer_size < length) {
        m_deflat_buffer_size = length;
        m_deflat_buffer.reset(new char[m_deflat_buffer_size]);
    }

    m_zlib_deflate_stream->next_in = reinterpret_cast<Bytef*>(const_cast<char*>(str));
    m_zlib_deflate_stream->next_out = reinterpret_cast<Bytef*>(m_deflat_buffer.get());
    m_zlib_deflate_stream->avail_in = length;
    m_zlib_deflate_stream->avail_out = m_deflat_buffer_size;
    int ret = deflate(m_zlib_deflate_stream, Z_FINISH);
    if (ret == Z_STREAM_END) {
        out->assign(m_deflat_buffer.get(), m_deflat_buffer_size - m_zlib_deflate_stream->avail_out);
        return true;
    } else {
        LOG(ERROR)<< "fail to Compress data!" << ret;
        return false;
    }
}

TOFT_REGISTER_BLOCK_COMPRESSION(GzipCompression, "gzip");
} // namespace toft
