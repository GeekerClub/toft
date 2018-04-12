// Copyright (C) 2016, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#include "toft/compress/block/stream_compression.h"

#include <string>

#include "thirdparty/glog/logging.h"
#include "thirdparty/gtest/gtest.h"
#include "toft/base/scoped_ptr.h"
#include "toft/storage/file/file.h"

namespace toft {

const std::string content1 = "abcdefghigddddddddddddddddddddddddddddddddddddd";
const std::string content2 = "123456789999999999999999999999999999999999";

TEST(StreamCompress, File) {
    std::string test_file = "./test_file";
    StreamCompression stream("lzo");

    scoped_ptr<File> fp(File::Open(test_file, "w"));
    EXPECT_TRUE(fp);

    std::string output;
    stream.AddBuffer(content1);
    EXPECT_TRUE(stream.Compress(&output));
    LOG(INFO) << "compress: from " << content1.length() << " to " << output.length();
    ASSERT_EQ(output.size(), fp->Write(output.data(), output.size()));

    output.clear();
    stream.AddBuffer(content2);
    EXPECT_TRUE(stream.Compress(&output));
    LOG(INFO) << "compress: from " << content2.length() << " to " << output.length();
    ASSERT_EQ(output.size(), fp->Write(output.data(), output.size()));

    EXPECT_TRUE(fp->Close());


    fp.reset(File::Open(test_file, "r"));
    std::vector<std::string> results;
    std::string result_str;
    while (!fp->IsEof()) {
        char buffer[5];
        int64_t read_count = fp->Read(buffer, sizeof(buffer));
        if (!fp->IsEof()) {
            EXPECT_EQ(5, read_count);
        }
        stream.AddBuffer(buffer, read_count);
        if (stream.Uncompress(&result_str)) {
            LOG(INFO) << "uncompress: " << result_str;
            results.push_back(result_str);
        } else {
            LOG(INFO) << "error msg: " << stream.ErrorString();
        }
        result_str.clear();
    }
    EXPECT_TRUE(fp->Close());
    EXPECT_EQ(2, results.size());
    EXPECT_EQ(content1, results[0]);
    EXPECT_EQ(content2, results[1]);
}

} // namespace toft
