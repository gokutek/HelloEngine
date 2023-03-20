#include "descriptor_heap.h"
#include "utility.h"
#include "graphics_core.h"

descriptor_heap::descriptor_heap() :
	descriptor_size_(0),
	free_descriptors_num_(0)
{
	memset(&heap_desc_, 0, sizeof(heap_desc_));
}

descriptor_heap::~descriptor_heap()
{
}

void descriptor_heap::create(const wchar_t* debug_heap_name, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t max_count)
{
	heap_desc_.Type = type;
	heap_desc_.NumDescriptors = max_count;
	heap_desc_.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heap_desc_.NodeMask = 1;

	ASSERT_SUCCEEDED(graphics::device->CreateDescriptorHeap(&heap_desc_, IID_PPV_ARGS(rhi_heap_.ReleaseAndGetAddressOf())));
	rhi_heap_->SetName(debug_heap_name);

	descriptor_size_ = graphics::device->GetDescriptorHandleIncrementSize(type);
	free_descriptors_num_ = max_count;

	first_handle_ = descriptor_handle(rhi_heap_->GetCPUDescriptorHandleForHeapStart(), rhi_heap_->GetGPUDescriptorHandleForHeapStart());
	next_free_handle_ = first_handle_;
}

void descriptor_heap::destroy()
{
	rhi_heap_ = nullptr;
}

descriptor_handle descriptor_heap::alloc(uint32_t count)
{
	ASSERT(has_available_space(count));
	descriptor_handle handle = next_free_handle_;
	next_free_handle_ += count * descriptor_size_;
	free_descriptors_num_ -= count;
	return handle;
}

bool descriptor_heap::is_valid_handle(const descriptor_handle & handle) const
{
	//检查是否越界
	if (handle.get_cpu_ptr() < first_handle_.get_cpu_ptr()
		|| handle.get_cpu_ptr() >= first_handle_.get_cpu_ptr() + heap_desc_.NumDescriptors * descriptor_size_)
	{
		return false;
	}

	//检查CPU与GPU的两个offset是否相等
	if (handle.get_cpu_ptr() - first_handle_.get_cpu_ptr() != handle.get_gpu_ptr() - first_handle_.get_gpu_ptr())
	{
		return false;
	}

	return true;
}
