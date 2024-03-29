﻿#pragma once

#include "pch.h"
#include <assert.h>
#include <stdio.h>

#ifdef _DEBUG
#define ASSERT(condition, ...) assert(condition)

#define WARN_ONCE_IF_NOT(condition, ...) assert(condition)

#define ASSERT_SUCCEEDED(hr, ... ) \
        if (FAILED(hr)) { \
            assert(false); \
        }
#else
#define ASSERT(condition, ...) condition
#define WARN_ONCE_IF_NOT(condition, ...) condition
#define ASSERT_SUCCEEDED(hr, ... ) hr
#endif

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace utility
{
	inline void Print(const char* msg)
	{
		OutputDebugStringA(msg);
	}

	inline void Print(const wchar_t* msg) 
	{
		OutputDebugString(msg);
	}

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

	/** 计算数组的哈希 */
	template<typename T>
	inline size_t hash_array(T const* array, size_t count, size_t hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State object is not word-aligned");

		uint32_t const* begin = (uint32_t const*)array;
		uint32_t const* end = (uint32_t const*)(array+count);
		for (; begin != end; ++begin)
		{
			hash = hash * 16777619U ^ *begin;
		}

		return hash;
	}
}
