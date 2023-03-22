#include "descriptor_allocator.h"
#include "graphics_core.h"
#include "utility.h"

static constexpr int NUM_DESCRIPTORS_PER_HEAP = 256;

std::mutex descriptor_allocator::mutex_;
std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> descriptor_allocator::heap_pool_;

descriptor_allocator::descriptor_allocator(D3D12_DESCRIPTOR_HEAP_TYPE heap_type) :
	heap_type_(heap_type),
	current_heap_(nullptr),
	free_handles_(0),
	descriptor_size_(0)
{
	memset(&current_handle_, 0, sizeof(current_handle_));
}

D3D12_CPU_DESCRIPTOR_HANDLE descriptor_allocator::allocate(uint32_t count)
{
	if (!current_heap_ || free_handles_ < count)
	{
		//再分配一个新的（当前heap的剩余空间会浪费掉）
		current_heap_ = request_new_heap(heap_type_);
		current_handle_ = current_heap_->GetCPUDescriptorHandleForHeapStart();
		free_handles_ = NUM_DESCRIPTORS_PER_HEAP;

		if (!descriptor_size_)
		{
			descriptor_size_ = graphics::device->GetDescriptorHandleIncrementSize(heap_type_);
		}
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = current_handle_;
	handle.ptr += count * descriptor_size_;
	free_handles_ -= count;
	return handle;
}

ID3D12DescriptorHeap* descriptor_allocator::request_new_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = heap_type;
	desc.NumDescriptors = NUM_DESCRIPTORS_PER_HEAP;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 1;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;
	ASSERT_SUCCEEDED(graphics::device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));

	std::lock_guard<std::mutex> guard(mutex_);
	heap_pool_.emplace_back(heap);
	return heap.Get();
}

void descriptor_allocator::destroy_all()
{
	//FIXME: 这里不需要加锁么?
	heap_pool_.clear();
}
