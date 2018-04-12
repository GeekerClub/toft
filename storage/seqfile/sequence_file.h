// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#ifndef TOFT_STORAGE_SEQFILE_SEQFILE_H_
#define TOFT_STORAGE_SEQFILE_SEQFILE_H_

#include "boost/scoped_ptr.hpp"
#include "toft/storage/file/hdfs_file.h"


namespace toft {

class SequenceFileReader;
class SequenceFileWriter;

class SequenceFile {
public:
    enum CompressionType {
        NONE,
        RECORD,
        BLOCK
    };

    SequenceFile();
    virtual ~SequenceFile();

    /**
     *  Create a reader to read from SequenceFile.
     *  @param uri uri for the file to read from, currently only support hdfs and local
     *  @return pointer to the reader, NULL if failed.
     */
    virtual SequenceFileReader* CreateReader(const std::string& uri);
    /**
     *  Create a writer to write to a SequenceFile
     *  @param uri uri for the file to write to, currently only support hdfs and local
     *  @param type compression type for the file writing
     *  @return pointer to the writer, NULL if failed.
     */
    virtual SequenceFileWriter* CreateWriter(const std::string& uri, CompressionType type,
            const std::string& codex_string);
     /**
     *  Create a writer to write to a SequenceFile by uri. Compression can be specified by
     *  URI, like:
     *   /hdfs/ecomon/xxx_path/seqfile.seq?compression_type=block,codex_type=bz2
     *  @param uri uri for the file to write to, currently only support hdfs and local
     *  @param type compression type for the file writing
     *  @return pointer to the writer, NULL if failed.
     */
    virtual SequenceFileWriter* CreateWriter(const std::string& uri);
private:
    hdfsFS GetHDFSFileSystem(const std::string& path);
};

} // namespace toft

#endif
