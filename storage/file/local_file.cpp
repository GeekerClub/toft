// Copyright (c) 2013, The Toft Authors.
// All rights reserved.
//
// Author: CHEN Feng <chen3feng@gmail.com>

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include "toft/storage/file/local_file.h"

#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "toft/base/string/algorithm.h"
#include "toft/base/unique_ptr.h"
#include "toft/storage/path/path.h"
#include "toft/text/wildcard.h"

namespace toft {
namespace {

class LocalFileIterator : public FileIterator {
public:
    explicit LocalFileIterator(DIR* dp, const std::string& dir,
                               const std::string& pattern,
                               int include_types, int exclude_types)
        : m_dp(dp, closedir), m_dir(dir), m_pattern(pattern),
          m_include_types(include_types), m_exclude_types(exclude_types) {}
    ~LocalFileIterator() {}

    bool GetNext(FileEntry* entry) {
        for (;;) {
            dirent* de = readdir(m_dp.get()); // NOLINT(runtime/threadsafe_fn)
            if (!de)
                return false;

            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
                continue;
            }

            int type = GetType(de);
            if ((type & m_include_types) == 0)
                continue;
            if ((type & m_exclude_types) != 0)
                continue;

            if (!Wildcard::Match(m_pattern, de->d_name))
                continue;

            entry->type = type;
            entry->name = de->d_name;
            return true;
        }
    }

private:
    int GetType(dirent* de) const {
        int type = FileType_None;
        if (de->d_type != DT_UNKNOWN) { // Not all filesystem support d_type
            switch (de->d_type) {
            case DT_REG:
                type |= FileType_Regular;
                break;
            case DT_DIR:
                type |= FileType_Directory;
                break;
            case DT_LNK:
                type |= FileType_Link;
                break;
            }
        } else {
            std::string path = Path::Join(m_dir, de->d_name);
            struct stat buf;
            if (lstat(path.c_str(), &buf) == 0) {
                if (S_ISREG(buf.st_mode))
                    type |= FileType_Regular;
                if (S_ISDIR(buf.st_mode))
                    type |= FileType_Directory;
                if (S_ISLNK(buf.st_mode))
                    type |= FileType_Link;
            }
        }
        return type;
    }

private:
    std::unique_ptr<DIR, int(*)(DIR*)> m_dp; // NOLINT
    std::string m_dir;
    std::string m_pattern;
    int m_include_types;
    int m_exclude_types;
};

} // namespace

/////////////////////////////////////////////////////////////////////////////
// LocalFileSystem

File* LocalFileSystem::Open(const std::string& file_path, const char* mode) {
    std::unique_ptr<FILE, int (*)(FILE*)> // NOLINT
        fp(fopen(file_path.c_str(), mode), &fclose);
    if (!fp)
        return NULL;
    return new LocalFile(fp.release(), file_path, mode);
}

bool LocalFileSystem::Exists(const std::string& file_path) {
    return access(file_path.c_str(), F_OK) == 0;
}

bool LocalFileSystem::Delete(const std::string& file_path) {
    return remove(file_path.c_str()) == 0;
}

bool LocalFileSystem::Mkdir(const std::string& path, int mode) {
    return mkdir(path.c_str(), mode) == 0;
}

bool LocalFileSystem::Rmdir(const std::string& path) {
    return rmdir(path.c_str()) == 0;
}

bool LocalFileSystem::Rename(const std::string& from, const std::string& to) {
    return rename(from.c_str(), to.c_str()) == 0;
}

bool LocalFileSystem::GetTimes(const std::string& file_path, FileTimes* times) {
    struct stat buf;
    int ret = stat(file_path.c_str(), &buf);
    if (ret < 0)
        return false;
    times->access_time = buf.st_atime;
    times->modify_time = buf.st_mtime;
    times->change_time = buf.st_ctime;
    return true;
}

int64_t LocalFileSystem::GetSize(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        return -1;
    }
    return st.st_size;
}

FileIterator* LocalFileSystem::Iterate(const std::string& dir,
                                       const std::string& pattern,
                                       int include_types,
                                       int exclude_types) {
    // NOLINT
    std::unique_ptr<DIR, int(*)(DIR*)> dp(opendir(dir.c_str()), closedir);
    if (!dp)
        return NULL;
    return new LocalFileIterator(dp.release(), dir, pattern, include_types,
                                 exclude_types);
}

TOFT_REGISTER_FILE_SYSTEM("local", LocalFileSystem);

/////////////////////////////////////////////////////////////////////////////
// LocalFile

LocalFile::LocalFile(FILE* fp, const std::string& file_path, const char* mode)
    : File(file_path, mode), m_fp(fp) {
}

LocalFile::~LocalFile()
{
    Close();
}

int64_t LocalFile::Read(void* buffer, int64_t size)
{
    int64_t nread = fread(buffer, 1, size, m_fp);
    if (ferror(m_fp)) return -1;
    return nread;
}

int64_t LocalFile::Write(const void* buffer,
                         int64_t size)
{
    size_t nwrite = fwrite(buffer, 1, size, m_fp);
    if (ferror(m_fp)) return -1;
    return nwrite;
    // int fd = fileno(m_fp);
    // return write(fd, buffer, size);
}

bool LocalFile::Flush()
{
    return fflush(m_fp) == 0;
    // int fd = fileno(m_fp);
    // return fsync(fd) == 0;
}

bool LocalFile::Close()
{
    if (m_fp == NULL)
        return true;
    FILE* fp = m_fp;
    m_fp = NULL;
    return fclose(fp) == 0;
}

bool LocalFile::Seek(int64_t offset, int whence)
{
    return fseeko(m_fp, offset, whence) == 0;
}

int64_t LocalFile::Tell()
{
    return ftello(m_fp);
}

bool LocalFile::ReadLine(std::string* line, size_t max_size)
{
    line->resize(max_size + 1);
    char* p = fgets(&(*line)[0], max_size, m_fp);
    if (!p)
        return false;
    line->resize(strlen(p));
    RemoveLineEnding(line);
    return true;
}

bool LocalFile::ReadLineWithLineEnding(std::string* line, size_t max_size)
{
    line->resize(max_size + 1);
    char* p = fgets(&(*line)[0], max_size, m_fp);
    if (!p)
        return false;
    line->resize(strlen(p));
    return true;
}

bool LocalFile::IsEof() {
    if (0 == feof(m_fp))  return false;
    return true;
}

} // namespace toft
