// Copyright (c) 2011, Tencent Inc.
// All rights reserved.
//

#ifndef  TOFT_ENCODING_CHARSET_CONVERTER_H_
#define  TOFT_ENCODING_CHARSET_CONVERTER_H_

// disallow define iconv to libiconv if libiconv is installed
#define LIBICONV_PLUG
#include <iconv.h>

#include <stddef.h>
#include <string>
#include "toft/base/string/string_piece.h"
#include "toft/base/uncopyable.h"

namespace toft {

/// i18n charset converter
/// @note CharsetConverter is not thread safe,
/// so it can not be used in multiple threads simultaneously
class CharsetConverter
{
    TOFT_DECLARE_UNCOPYABLE(CharsetConverter);
public:
    /// Create a immediate usable converter, if fail, fatal error will be
    /// raised.
    /// @param from encoding name to be converter from
    /// @param to encoding name to be converter to
    CharsetConverter(const StringPiece& from, const StringPiece& to);

    /// Create an empty and unusable converter, explicit Create should be
    /// called before use it
    CharsetConverter();
    ~CharsetConverter();

    /// Create a converter
    /// @param from encoding name to be converter from
    /// @param to encoding name to be converter to
    bool Create(const StringPiece& from, const StringPiece& to);

    /// convert string to another charset, append the result to *out
    /// @param in input string
    /// @param out converted result will be appended to this
    /// @param converted_size pointer to a size_t to receive converted byte size
    /// @returns whether all bytes converted successfully
    bool ConvertAppend(
        const StringPiece& in,
        std::string* out,
        size_t* converted_size = NULL);

    /// convert string to another charset
    /// @param in input string
    /// @param out to receive converted result
    /// @param converted_size pointer to a size_t to receive converted byte size
    /// @returns whether all bytes converted successfully
    bool Convert(
        const StringPiece& in,
        std::string* out,
        size_t* converted_size = NULL);
private:
    iconv_t m_cd;
};

/////////////////////////////////////////////////////////////////////////////
/// convert GBK string to UTF-8 string
inline bool ConvertGbkToUtf8(const StringPiece& in, std::string* out)
{
    return CharsetConverter("GBK", "UTF-8").Convert(in, out);
}

/// convert UTF-8 string to GBK string
inline bool ConvertUtf8ToGbk(const StringPiece& in, std::string* out)
{
    return CharsetConverter("UTF-8", "GBK").Convert(in, out);
}

} // namespace toft

#endif  // TOFT_ENCODING_CHARSET_CONVERTER_H_
