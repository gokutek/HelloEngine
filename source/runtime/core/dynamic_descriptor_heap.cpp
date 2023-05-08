#include "dynamic_descriptor_heap.h"
#include "root_signature.h"
#include "descriptor_heap.h"
#include "graphics_core.h"
#include "utility.h"

dynamic_descriptor_heap::dynamic_descriptor_heap(command_context* owning_context, D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
	: owning_context_(owning_context),
	descriptor_heap_type_(heap_type),
	descriptor_size_(get_rhi()->device->GetDescriptorHandleIncrementSize(heap_type)),
	current_heap_(nullptr),
	current_offset_(0)
{
}

dynamic_descriptor_heap::~dynamic_descriptor_heap()
{
}

void dynamic_descriptor_heap::cleanup_used_heaps()
{
	//TODO:
}

void dynamic_descriptor_heap::set_graphics_descriptor_handles(uint32_t root_index, uint32_t offset, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
	//TODO:
}

void dynamic_descriptor_heap::set_compute_descriptor_handles(uint32_t root_index, uint32_t offset, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
	//TODO:
}

D3D12_GPU_DESCRIPTOR_HANDLE dynamic_descriptor_heap::upload_direct(D3D12_CPU_DESCRIPTOR_HANDLE handles)
{
	return D3D12_GPU_DESCRIPTOR_HANDLE();
}

void dynamic_descriptor_heap::parse_graphics_root_signature(root_signature const& root_sig)
{
	//TODO:
}

void dynamic_descriptor_heap::parse_compute_root_signature(root_signature const& root_sig)
{
	//TODO:
}

void dynamic_descriptor_heap::commit_graphics_root_descriptor_tables(ID3D12GraphicsCommandList* cmd_list)
{
	//TODO:
}

void dynamic_descriptor_heap::commit_compute_root_descriptor_tables(ID3D12GraphicsCommandList* cmd_list)
{
	//TODO:
}

ID3D12DescriptorHeap* dynamic_descriptor_heap::request_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type)
{
	std::lock_guard<std::mutex> gurad(mutex_);
	uint32_t idx = (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0);
	
	// 等待GPU用完后，从retired_descriptor_heaps_转移到available_descriptor_heaps_
	while (!retired_descriptor_heaps_[idx].empty() && get_rhi()->command_manager_.is_fence_complete(retired_descriptor_heaps_[idx].front().first))
	{
		available_descriptor_heaps_[idx].push(retired_descriptor_heaps_[idx].front().second);
		retired_descriptor_heaps_[idx].pop();
	}

	// 优先复用之前创建的
	if (!available_descriptor_heaps_[idx].empty())
	{
		ID3D12DescriptorHeap* heap = available_descriptor_heaps_[idx].front();
		available_descriptor_heaps_[idx].pop();
		return heap;
	}

	// 无可复用的，创建一个新的
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
	heap_desc.Type = heap_type;
	heap_desc.NumDescriptors = kNumDescriptorsPerHeap;
	heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heap_desc.NodeMask = 1;
	ComPtr<ID3D12DescriptorHeap> descriptor_heap;
	ASSERT_SUCCEEDED(get_rhi()->device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&descriptor_heap)));
	descriptor_heap_pool_[idx].emplace_back(descriptor_heap);
	return descriptor_heap.Get();
}

void dynamic_descriptor_heap::discard_descriptor_heaps(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint64_t fence_value_for_reset, std::vector<ID3D12DescriptorHeap*> const& used_heaps)
{
	std::lock_guard<std::mutex> gurad(mutex_);
	uint32_t idx = (heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? 1 : 0);

	for (ID3D12DescriptorHeap* heap : used_heaps)
	{
		retired_descriptor_heaps_[idx].push(std::make_pair(fence_value_for_reset, heap));
	}
}

bool dynamic_descriptor_heap::has_space(uint32_t count) const
{
	if (!current_heap_) { return false; }

	return current_offset_ + count <= kNumDescriptorsPerHeap;
}

void dynamic_descriptor_heap::retire_current_heap()
{
	if (!current_offset_ || !current_heap_)
	{
		return;
	}

	retired_heaps_.push_back(current_heap_);
	current_heap_ = nullptr;
	current_offset_ = 0;
}

void dynamic_descriptor_heap::retire_used_heaps(uint64_t fence_value)
{
	discard_descriptor_heaps(descriptor_heap_type_, fence_value, retired_heaps_);
	retired_heaps_.clear();
}

ID3D12DescriptorHeap* dynamic_descriptor_heap::get_heap_pointer()
{
	if (!current_heap_)
	{
		current_heap_ = request_descriptor_heap(descriptor_heap_type_);
		first_descriptor_handle_ = descriptor_handle(current_heap_->GetCPUDescriptorHandleForHeapStart(), current_heap_->GetGPUDescriptorHandleForHeapStart());
		current_offset_ = 0;
	}

	return current_heap_;
}

descriptor_handle dynamic_descriptor_heap::allocate(uint32_t count)
{
	descriptor_handle handle = first_descriptor_handle_ + count * descriptor_size_;
	current_offset_ += count;
	return handle;
}

void dynamic_descriptor_heap::copy_and_bind_staged_tables(descriptor_handle_cache& handle_cache, ID3D12GraphicsCommandList* cmd_list, void(STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
{
	//TODO:
}

void dynamic_descriptor_heap::unbind_all_valid()
{
	//TODO:
}
