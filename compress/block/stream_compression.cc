// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//


#include "toft/base/string/algorithm.h"
#include "toft/compress/block/stream_compression.h"

namespace toft {

StreamCompression::StreamCompression(const std::string& name) {
    m_compression.reset(TOFT_CREATE_BLOCK_COMPRESSION(name));
    if (m_compression.get() == NULL) {
        m_compression.reset(TOFT_CREATE_BLOCK_COMPRESSION("none"));
    }
}

StreamCompression::~StreamCompression() {}

bool StreamCompression::AddBuffer(const char* buffer, size_t size) {
    return AddBuffer(std::string(buffer, size));
}

bool StreamCompression::AddBuffer(const std::string& buffer) {
    if (m_compression->GetName() == "none") {
        return true;
    }
    MutexLocker lock(&m_mutex);
    m_buffer += buffer;
    return true;
}

bool StreamCompression::Compress(std::string* output) {
    const size_t segment_size = 128 * 1024;
    std::vector<std::string> segments;

    MutexLocker lock(&m_mutex);
    if (m_compression->GetName() == "none") {
        *output = m_buffer;
        m_buffer.clear();
        return true;
    }

    const char* pos = m_buffer.data();
    size_t size = m_buffer.size();
    while (size > 0) {
        uint32_t raw_len = segment_size;
        if (size < segment_size) {
            raw_len = size;
        }
        std::string compressed_str;
        if (!m_compression->Compress(pos, raw_len, &compressed_str)) {
            return false;
        }

        std::string raw_len_str;
        PutFixed32(raw_len, &raw_len_str);
        segments.push_back(raw_len_str);
        std::string compressed_len_str;
        PutFixed32(compressed_str.size(), &compressed_len_str);
        segments.push_back(compressed_len_str);
        segments.push_back(compressed_str);

        size -= raw_len;
        pos += raw_len;
    }
    StringPiece blank_delim;
    JoinStrings(segments, blank_delim, output);
    m_buffer.clear();

    return true;
}

bool StreamCompression::Uncompress(std::string* output) {
    MutexLocker lock(&m_mutex);
    if (m_compression->GetName() == "none") {
        *output = m_buffer;
        m_buffer.clear();
        return true;
    }

    const char* pos = m_buffer.data();
    size_t size = m_buffer.size();
    std::vector<std::string> segments;
    while (pos < m_buffer.data() + m_buffer.size()) {
        m_err.clear();
        std::string decompressed_str;
        size_t consumed_len = 0;
        if (!UncompressPrefix(pos, size, &decompressed_str, &consumed_len)) {
            break;
        }
        segments.push_back(decompressed_str);
        pos += consumed_len;
        size -= consumed_len;
    }
    if (segments.size() == 0) {
        return false;
    }
    m_buffer = m_buffer.substr(m_buffer.size() - size);
    StringPiece blank_delim;
    JoinStrings(segments, blank_delim, output);
    return true;
}

bool StreamCompression::UncompressPrefix(const char* buffer, size_t size,
                                         std::string* output, size_t *consumed_len) {
    *consumed_len = 0;
    const int32_t HEADER_SIZE = sizeof(uint32_t) * 2;
    if (size < HEADER_SIZE) {
        m_err = "string too small";
        return false;
    }
    const char* pos = buffer;
    uint32_t uncompressed_len;
    uint32_t raw_len;
    GetFixed32(pos, &uncompressed_len);
    GetFixed32(pos + sizeof(uint32_t), &raw_len);

    if (pos + HEADER_SIZE + raw_len > buffer + size) {
        m_err = "parse header fail";
        return false;
    }
    if (!m_compression->Uncompress(pos + HEADER_SIZE, raw_len, output)) {
        m_err = "uncompress fail";
        return false;
    }

    if (uncompressed_len != output->size()) {
        m_err = "uncompress len not matched";
        return false;
    }
    *consumed_len = raw_len + HEADER_SIZE;
    return true;
}

void StreamCompression::PutFixed32(uint32_t value, std::string* dst) {
    char buf[sizeof(uint32_t)];
    memcpy(buf, &value, sizeof(uint32_t));
    dst->append(buf, sizeof(buf));
}

void StreamCompression::GetFixed32(const char* pos, uint32_t* value) {
    *value = 0;
    memcpy(value, pos, sizeof(uint32_t));
}

} // namespace toft
