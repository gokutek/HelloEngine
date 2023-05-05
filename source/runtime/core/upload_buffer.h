#pragma once

#include "gpu_resource.h"

/**
 * @brief 该Buffer可由CPU写、GPU读，典型的用处是用来存储顶点数据。
 * 
 * 常见用法：
 * @code
 * upload_buffer buffer;
 * buffer.create(L"vertex buffer", sizeof(vertex) * vertex_count);
 * void* data = buffer.map();
 * memcpy(data, sz, vertex_data);
 * buffer.unmap(0);
 * @endcode
 */
class upload_buffer : public gpu_resource
{
public:
	upload_buffer();

	/**
	 * @brief 创建资源
	 */
	int create(wchar_t const* name, size_t buffer_size);

	/**
	 * @brief 映射资源，使得CPU可读取该资源
	 */
	void* map();

	/**
	 * @brief 对应上面的map调用
	 */
	void unmap(size_t begin, size_t end = -1);

	/**
	 * @brief 获取资源大小
	 */
	size_t get_buffer_size() const;

private:
	size_t buffer_size_; ///< 
};

inline size_t upload_buffer::get_buffer_size() const
{
	return buffer_size_;
}
