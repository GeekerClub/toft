// Copyright (C) 2012, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:


#ifndef TOFT_STORAGE_PATH_PATH_EXT_H
#define TOFT_STORAGE_PATH_PATH_EXT_H

#include <string>
#include <vector>

namespace toft {

void SplitStringPath(const std::string& full_path,
                     std::string* dir_part,
                     std::string* file_part);

std::string ConcatStringPath(const std::vector<std::string>& sections,
                             const std::string& delim = ".");

std::string GetPathPrefix(const std::string& full_path,
                          const std::string& delim = "/");

bool CreateDirWithRetry(const std::string& dir_path);

bool ListCurrentDir(const std::string& dir_path,
                    std::vector<std::string>* file_list);

bool IsExist(const std::string& path);

bool IsDir(const std::string& path);

bool RemoveLocalFile(const std::string& path);

bool MoveLocalFile(const std::string& src_file,
                   const std::string& dst_file);

} // namespace toft
#endif // TOFT_STORAGE_PATH_PATH_EXT_H
