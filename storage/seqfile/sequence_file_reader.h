// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#ifndef TOFT_STORAGE_SEQFILE_SEQFILE_READRE_H_
#define TOFT_STORAGE_SEQFILE_SEQFILE_READRE_H_

#include <stddef.h>
#include <string>
#include "toft/base/string/string_piece.h"
#include "toft/storage/seqfile/sequence_file.h"

namespace toft {

class SequenceFileReader {
public:
    virtual ~SequenceFileReader();

    /**
     * Read a record into file.
     * @param key key string
     * @param value value string
     * @return whether succeeded or not
     */
    virtual bool ReadRecord(std::string* key, std::string* value);
    /**
     * Read a record into file, StringPiece manner. Returns 2 StringPiece pointing to
     * the data buffer hold by libhdfs. Enables user to peek into record without copying
     * to a string.
     * @param key key StringPiece
     * @param value value StringPiece
     * @return whether succeeded or not
     */
    virtual bool ReadRecord(StringPiece* key, StringPiece* value);
    /**
     * Skip to the next sync marker.
     * @return whether succeeded or not
     */
    virtual size_t SkipToSync(size_t offset);
    /**
     * Seek to a offset in the file.
     * @param offset to seek to
     * @return whether succeeded or not
     */
    virtual bool Seek(size_t offset);
    /**
     * Tell the current position in the file in bytes.
     * @return offset in bytes.
     */
    virtual int64_t Tell();
    /**
     * Close the file. No return value.
     */
    virtual void Close();

    bool is_finished() { return m_finished; };
private:
    // private c'tor, only create from SequenceFile.
    friend class SequenceFile;
    SequenceFileReader(hdfsFS fs, SeqFile file);

    hdfsFS m_fs;
    SeqFile m_file;
    bool m_finished;
};

}

#endif
