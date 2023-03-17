#pragma once

#include "pch.h"
#include <assert.h>
#include <stdio.h>

#define ASSERT_SUCCEEDED(hr, ... ) \
        if (FAILED(hr)) { \
            assert(false); \
        }

#define ASSERT(condition, ...) assert(condition)

#define WARN_ONCE_IF_NOT(condition, ...)

namespace utility
{
	inline void Print(const char* msg) { OutputDebugStringA(msg); }
	inline void Print(const wchar_t* msg) { OutputDebugString(msg); }

	inline void Printf(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}

	inline void Printf(const wchar_t* format, ...)
	{
		wchar_t buffer[256];
		va_list ap;
		va_start(ap, format);
		vswprintf(buffer, 256, format, ap);
		va_end(ap);
		Print(buffer);
	}
}
