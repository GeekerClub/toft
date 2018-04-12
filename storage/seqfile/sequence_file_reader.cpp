// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#include "sequence_file_reader.h"

namespace toft {

using namespace std;

SequenceFileReader::SequenceFileReader(hdfsFS fs, SeqFile file) :
        m_fs(fs),
        m_file(file),
        m_finished(false) {
}

SequenceFileReader::~SequenceFileReader() {
}

bool SequenceFileReader::ReadRecord(std::string* key, std::string* value) {
    StringPiece key_piece;
    StringPiece value_piece;
    if (!ReadRecord(&key_piece, &value_piece)) {
        return false;
    }

    key_piece.copy_to_string(key);
    value_piece.copy_to_string(value);
    return true;
}

bool SequenceFileReader::ReadRecord(StringPiece* key, StringPiece* value) {
    if (m_finished) {
        return false;
    }

    void* key_data;
    int key_len;
    void* value_data;
    int value_len;

    int status = readNextRecordFromSeqFile(m_fs, m_file,
            &key_data, &key_len, &value_data, &value_len);
    if (status == 1) {
        m_finished = true;
    }
    if (status != 0) {
        return false;
    }

    key->set(key_data, key_len);
    value->set(value_data, value_len);
    return true;

}

size_t SequenceFileReader::SkipToSync(size_t offset) {
    return syncSeqFile(m_file, offset);
}

bool SequenceFileReader::Seek(size_t offset) {
    return seekSeqFile(m_file, offset) == 0;
}
int64_t SequenceFileReader::Tell() {
    return getSeqFilePos(m_file);
}

void SequenceFileReader::Close() {
    closeSequenceFile(m_fs, m_file);
}

} // namespace toft
