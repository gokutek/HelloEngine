#pragma once

#include "gpu_resource.h"

/*
===============================================================================
��Buffer����CPUд��GPU�������͵��ô��������洢�������ݡ�
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
