// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#include "toft/storage/seqfile/sequence_file.h"

#include <string>
#include <vector>
#include "glog/logging.h"
#include "thirdparty/hadoop/hdfs.h"
#include "toft/storage/file/uri_utils.h"
#include "toft/storage/seqfile/sequence_file_reader.h"
#include "toft/storage/seqfile/sequence_file_writer.h"

namespace toft {

using namespace std;

hdfsFS SequenceFile::GetHDFSFileSystem(const std::string& file_path) {
    std::vector<std::string> sections;
    if (!UriUtils::Explode(file_path, '/', &sections)) {
        return NULL;
    }
    // handle local files
    if (sections[0] != "hdfs") {
        return hdfsConnect(NULL, 0);
    }
    std::string cluster_name;

    map<string, string> params;

    if (!UriUtils::ParseParam(sections[1], &cluster_name, &params)) {
        return NULL;
    }
    hdfsFS fs;

    vector<string> split;
    if (!UriUtils::Explode(cluster_name, ':', &split)) {
        return false;
    }
    string host = split[0];
    int port = atoi(split[1].c_str());
    std::string username;
    std::string password;
    // if there is username/password in params
    if (params.count("username") == 1) {
        std::map<std::string, std::string>::iterator it;
        it = params.find("username");
        username = it->second;
        it = params.find("password");
        password = it->second;
        if (it == params.end()) {
            return NULL;
        }
        return hdfsConnectAsUser(host.c_str(), port,
                username.c_str(), password.c_str());
    }
    return hdfsConnect(host.c_str(), port);
}

SequenceFile::SequenceFile() {
}

SequenceFile::~SequenceFile() {
}

SequenceFileReader* SequenceFile::CreateReader(const std::string& uri) {
    hdfsFS fs = GetHDFSFileSystem(uri);
    vector<string> sections;
    if (!UriUtils::Explode(uri, '/', &sections)) {
        return NULL;
    }
    std::string path = uri;
    if (sections[0] == "hdfs") {
        UriUtils::Shift(uri, &path, 2, '/');
    }
    SeqFile file = readSequenceFile(fs, path.c_str());
    if (file == NULL) {
        return NULL;
    }

    return new SequenceFileReader(fs, file);
}

SequenceFileWriter* SequenceFile::CreateWriter(const std::string& uri,
        CompressionType type, const std::string& codex_string) {
    std::string compression_type;
    switch(type) {
    case NONE:
        compression_type = "NONE";
    case RECORD:
        compression_type = "RECORD";
    case BLOCK:
        compression_type = "BLOCK";
    default:
        compression_type = "NONE";
    }

    hdfsFS fs = GetHDFSFileSystem(uri);
    vector<string> sections;
    if (!UriUtils::Explode(uri, '/', &sections)) {
        return NULL;
    }
    std::string path = uri;
    if (sections[0] == "hdfs") {
        UriUtils::Shift(uri, &path, 2, '/');
    }
    SeqFile file = writeSequenceFile(fs, path.c_str(),
            compression_type.c_str(), codex_string.c_str());

    return new SequenceFileWriter(fs, file);
}

SequenceFileWriter* SequenceFile::CreateWriter(const std::string& uri) {
    std::string compression_type = "NONE";
    std::string codec = "org.apache.hadoop.io.compress.DefaultCodec";
    hdfsFS fs = GetHDFSFileSystem(uri);
    vector<string> sections;
    if (!UriUtils::Explode(uri, '/', &sections)) {
        LOG(ERROR) << "failed to parse uri: " << uri;
        return NULL;
    }
    // parse options from last section of the path
    map<string, string> params;
    string filename;
    string last_section = sections[sections.size() - 1];
    if (!UriUtils::ParseParam(last_section, &filename, &params)) {
        return NULL;
    }
    std::string path = uri;
    if (sections[0] == "hdfs") {
        UriUtils::Shift(uri, &path, 2, '/');
    }
    if (params.count("compression_type") == 1) {
        compression_type = params["compression_type"];
    }
    if (params.count("codec") == 1) {
        codec = params["codec"];
    }
    path = path.substr(0, path.size() - last_section.size() + filename.size());
    SeqFile file = writeSequenceFile(fs, path.c_str(),
            compression_type.c_str(), codec.c_str());

    return new SequenceFileWriter(fs, file);
}

}
