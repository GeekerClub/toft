// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//

#include "toft/encoding/charset_converter.h"
#include "gtest/gtest.h"

using namespace toft;
TEST(CharsetConverter, Empty)
{
    CharsetConverter utf8_to_gbk("UTF-8", "GBK");
    std::string converted;
    utf8_to_gbk.Convert("", &converted);
}

TEST(CharsetConverter, Test)
{
    CharsetConverter utf8_to_gbk("UTF-8", "GBK");
    std::string converted;
    size_t converted_size;
    ASSERT_TRUE(utf8_to_gbk.Convert("Hello", &converted, &converted_size));
    EXPECT_EQ("Hello", converted);
    EXPECT_EQ(5U, converted_size);
    ASSERT_TRUE(utf8_to_gbk.ConvertAppend("Hello", &converted));
    EXPECT_EQ("HelloHello", converted);
    EXPECT_EQ(5U, converted_size);
}

TEST(CharsetConverter, BadEncodingName)
{
    CharsetConverter cc;
    EXPECT_FALSE(cc.Create("hello", "world"));
    EXPECT_FALSE(cc.Create("UTF-8", "world"));
    EXPECT_FALSE(cc.Create("hello", "UTF-8"));
}

static void CreateWithBadEncodingName(const char* from, const char* to)
{
    CharsetConverter cc(from, to);
}

TEST(CharsetConverter, BadEncodingNameDeadTest)
{
    EXPECT_ANY_THROW(CreateWithBadEncodingName("hello", "world"));
    EXPECT_ANY_THROW(CreateWithBadEncodingName("UTF-8", "world"));
    EXPECT_ANY_THROW(CreateWithBadEncodingName("hello", "UTF-8"));
}

// "中文转换测试" in UTF-8
const char kUtf8Text[] =
    "\xe4\xb8\xad\xe6\x96\x87\xe8\xbd\xac\xe6\x8d\xa2\xe6\xb5\x8b\xe8\xaf\x95\x0a";

// "中文转换测试" in GBK
const char kGbkText[] = "\xd6\xd0\xce\xc4\xd7\xaa\xbb\xbb\xb2\xe2\xca\xd4\x0a";

TEST(CharsetConverter, Utf8ToGbk)
{
    CharsetConverter utf8_to_gbk("UTF-8", "GBK");
    std::string converted;
    ASSERT_TRUE(utf8_to_gbk.Convert(kUtf8Text, &converted));
    EXPECT_EQ(kGbkText, converted);
    ASSERT_FALSE(utf8_to_gbk.Convert(kGbkText, &converted));
}

TEST(CharsetConverter, GbkToUtf8)
{
    CharsetConverter gbk_to_utf8("GBK", "UTF-8");
    std::string converted;
    ASSERT_TRUE(gbk_to_utf8.Convert(kGbkText, &converted));
    EXPECT_EQ(kUtf8Text, converted);
    ASSERT_FALSE(gbk_to_utf8.Convert(kUtf8Text, &converted));
}

