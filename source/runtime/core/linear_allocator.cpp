#include "linear_allocator.h"
#include "linear_allocation_page.h"
#include "linear_allocator_page_manager.h"

linear_allocator::linear_allocator(linear_allocator_type type)
	: allocator_type_(type),
	page_size_(type == linear_allocator_type::CpuWritable ? kCpuAllocatorPageSize : kGpuAllocatorPageSize),
	cur_offset_(0),
	cur_page_(nullptr)
{
}

dyn_alloc linear_allocator::allocate(size_t size, size_t alignment)
{
	//TODO:
	return *(dyn_alloc*)0;
}

void linear_allocator::cleanup_used_pages(uint64_t fence_value)
{
}
