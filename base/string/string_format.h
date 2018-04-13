// Copyright (C) 2013, For authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:

#ifndef TOFT_BASE_STRING_FORMAT_H
#define TOFT_BASE_STRING_FORMAT_H
#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <string>

namespace toft {

size_t StringFormatAppendVA(std::string* dst, const char* format, va_list ap);

size_t StringFormatAppend(std::string* dst, const char* format, ...);

size_t StringFormatTo(std::string* dst, const char* format, ...);

std::string StringFormat(const char* format, ...);


} // namespace toft

#endif // TOFT_BASE_STRING_FORMAT_H
