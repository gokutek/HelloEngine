#pragma once

#include "gpu_resource.h"

enum class linear_allocator_type
{
    GpuExclusive,       ///< D3D12_HEAP_TYPE_DEFAULT类型的堆，GPU-writeable (via UAV)
    CpuWritable,		///< D3D12_HEAP_TYPE_UPLOAD类型的堆，CPU-writeable (but write combined)
};

static constexpr size_t kGpuAllocatorPageSize = 0x10000;	// 64K
static constexpr size_t kCpuAllocatorPageSize = 0x200000;	// 2MB

struct dyn_alloc
{
    dyn_alloc(gpu_resource& BaseResource, size_t ThisOffset, size_t ThisSize)
        : buffer(BaseResource), 
        offset(ThisOffset),
        size(ThisSize),
        data(nullptr),
        gpu_address(D3D12_GPU_VIRTUAL_ADDRESS_NULL)
    {
    }

    gpu_resource& buffer;	// The D3D buffer associated with this memory.
    size_t offset;			// Offset from start of buffer resource
    size_t size;			// Reserved size of this allocation
    void* data;			// The CPU-writeable address
    D3D12_GPU_VIRTUAL_ADDRESS gpu_address;	// The GPU-visible address
};

class linear_allocation_page;
class linear_allocator_page_manager;

/**
 * @brief GPU内存分配管理器
 */
class linear_allocator
{
public:
	linear_allocator(linear_allocator_type type);
	dyn_alloc allocate(size_t size, size_t alignment = 256);
	void cleanup_used_pages(uint64_t fence_value);

private:
    dyn_alloc allocate_large_page(size_t size);

private:
    linear_allocator_type const allocator_type_; ///< 类型
    size_t const page_size_; ///< 页面大小

    size_t cur_offset_; ///< 未分配空间在当前页内的偏移
    linear_allocation_page* cur_page_; ///< 当前分配的页
    
    std::vector<linear_allocation_page*> retired_pages_; ///< 已用完的页面
    std::vector<linear_allocation_page*> large_page_list_; ///< 大页面
};
