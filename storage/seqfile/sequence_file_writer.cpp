// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#include "sequence_file_writer.h"

namespace toft {
SequenceFileWriter::SequenceFileWriter(hdfsFS fs, SeqFile file) :
        m_fs(fs),
        m_file(file) {
}

SequenceFileWriter::~SequenceFileWriter() {
}

bool SequenceFileWriter::WriteRecord(const std::string& key, const std::string& value) {
    return writeRecordIntoSeqFile(m_fs, m_file, key.c_str(), key.size(),
            value.c_str(), key.size()) == 0;
}

bool SequenceFileWriter::WriteRecord(const StringPiece& key, const StringPiece& value) {
    return writeRecordIntoSeqFile(m_fs, m_file, key.data(), key.size(),
            value.data(), key.size()) == 0;
}

bool SequenceFileWriter::Sync() {
    return syncSeqFileFs(m_fs, m_file) == 0;
}

size_t SequenceFileWriter::Tell() {
    return getSeqFilePos(m_file);
}

void SequenceFileWriter::Close() {
    closeSequenceFile(m_fs, m_file);
}

} //namespace toft
