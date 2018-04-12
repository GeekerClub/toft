// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#ifndef TOFT_STORAGE_SEQFILE_SEQFILE_WRITER_H_
#define TOFT_STORAGE_SEQFILE_SEQFILE_WRITER_H_

#include <string>

#include "thirdparty/hadoop/hdfs.h"
#include "toft/base/string/string_piece.h"
#include "toft/storage/seqfile/sequence_file.h"

namespace toft {

class SequenceFileWriter {
public:
    virtual ~SequenceFileWriter();

    /**
     * Write a record into file.
     * @param key key string
     * @param value value string
     * @return whether succeeded or not
     */
    virtual bool WriteRecord(const std::string& key, const std::string& value);
    /**
     * Write a record into file, StringPiece manner.
     * @param key key StringPiece
     * @param value value StringPiece
     * @return whether succeeded or not
     */
    virtual bool WriteRecord(const StringPiece& key, const StringPiece& value);

    /**
     * Write a sync marker into file.
     * @return whether succeeded or not
     */
    virtual bool Sync();
    /**
     * Tell the current position in the file in bytes.
     * @return offset in bytes.
     */
    virtual size_t Tell();
    /**
     * Close the file. No return value.
     */
    virtual void Close();
private:
    // private c'tor, only create from SequenceFile.
    friend class SequenceFile;
    SequenceFileWriter(hdfsFS fs, SeqFile file);

    hdfsFS m_fs;
    SeqFile m_file;
};

} //namespace toft

#endif
