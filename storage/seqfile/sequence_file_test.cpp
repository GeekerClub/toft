// Copyright 2013, For toft authors.
// Author: An Qin (anqin.qin@gmail.com)
// An Implementation for SequenceFile using libhdfs

#include "toft/storage/seqfile/sequence_file.h"
#include "toft/storage/seqfile/sequence_file_writer.h"
#include "toft/storage/seqfile/sequence_file_reader.h"
#include "toft/storage/file/file.h"
#include "toft/base/string/string_piece.h"

#include "gtest/gtest.h"
namespace {
using namespace toft;
using namespace std;

TEST(SequenceFileTest, NormalWrite) {
    SequenceFile file;
    SequenceFileWriter* writer = file.CreateWriter("test");
    ASSERT_TRUE(writer != NULL);
    std::string key = "abc";
    std::string value = "123";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    key = "def";
    value = "456";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    StringPiece key_piece("ghi");
    StringPiece value_piece("789");
    EXPECT_TRUE(writer->WriteRecord(key_piece, value_piece));
    writer->Close();
    delete writer;

    SequenceFileReader* reader = file.CreateReader("test");
    ASSERT_TRUE(reader != NULL);
    ASSERT_TRUE(reader->ReadRecord(&key, &value));
    ASSERT_TRUE(reader->ReadRecord(&key_piece, &value_piece));
    EXPECT_STREQ("def", key_piece.as_string().c_str());
    EXPECT_STREQ("456", value_piece.as_string().c_str());
    ASSERT_TRUE(reader->ReadRecord(&key, &value));
    EXPECT_STREQ("ghi", key.c_str());
    EXPECT_STREQ("789", value.c_str());
    ASSERT_FALSE(reader->ReadRecord(&key, &value));
    EXPECT_TRUE(reader->is_finished());
    reader->Close();
    delete reader;
}

TEST(SequenceFileTest, WriteSync) {
    SequenceFile file;
    SequenceFileWriter* writer = file.CreateWriter("test");
    ASSERT_TRUE(writer != NULL);
    string key = "abc";
    string value = "value";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    key = "def"; value = "456";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    key = "ghi"; value = "789";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    //EXPECT_TRUE(writer->Sync());
    key = "jkl"; value = "012";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    key = "mno"; value = "345";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    key = "pqr"; value = "678";
    EXPECT_TRUE(writer->WriteRecord(key, value));
    writer->Close();
    delete writer;

    SequenceFileReader* reader = file.CreateReader("test");
    ASSERT_TRUE(reader != NULL);
    ASSERT_GT(reader->SkipToSync(20), 95);
    ASSERT_TRUE(reader->ReadRecord(&key, &value));
    ASSERT_TRUE(reader->ReadRecord(&key, &value));
    EXPECT_STREQ("def", key.c_str());
    EXPECT_STREQ("456", value.c_str());
    ASSERT_TRUE(reader->ReadRecord(&key, &value));
    reader->Close();
    delete reader;
}



}
