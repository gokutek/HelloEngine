#pragma once

#include "gpu_resource.h"

/*
===============================================================================
该Buffer可由CPU写、GPU读，典型的用处是用来存储顶点数据。
===============================================================================
*/
class upload_buffer : public gpu_resource
{
public:
	upload_buffer();
	int create(wchar_t const* name, size_t buffer_size);
	void* map();
	void unmap(size_t begin, size_t end = -1);
	size_t get_buffer_size() const;

private:
	size_t buffer_size_;
};

inline size_t upload_buffer::get_buffer_size() const
{
	return buffer_size_;
}
