#include "linear_allocator_page_manager.h"
#include "graphics_core.h"
#include "utility.h"

linear_allocator_page_manager::linear_allocator_page_manager(linear_allocator_type allocation_type)
	: allocation_type_(allocation_type)
{
}

linear_allocation_page* linear_allocator_page_manager::request_page()
{
	std::lock_guard<std::mutex> guard(mutex_);

	//收集可复用的页
	while (!retired_pages_.empty() && get_rhi()->command_manager_.is_fence_complete(retired_pages_.front().first))
	{
		available_pages_.push(retired_pages_.front().second);
		retired_pages_.pop();
	}

	//使用复用的页
	if (!available_pages_.empty())
	{
		linear_allocation_page* page = available_pages_.front();
		available_pages_.pop();
		return page;
	}

	//创建新页
	linear_allocation_page* page = create_new_page(0);
	page_pool_.emplace_back(page);
	return page;
}

linear_allocation_page* linear_allocator_page_manager::create_new_page(size_t page_size)
{
	D3D12_HEAP_PROPERTIES heap_pops = {};
	heap_pops.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_pops.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_pops.CreationNodeMask = 1;
	heap_pops.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resource_desc = {};
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resource_desc.Alignment = 0;
	resource_desc.Height = 1;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.MipLevels = 1;
	resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	D3D12_RESOURCE_STATES initial_resource_state;

	if (allocation_type_ == linear_allocator_type::GpuExclusive)
	{
		heap_pops.Type = D3D12_HEAP_TYPE_DEFAULT;
		resource_desc.Width = (page_size ? page_size : kGpuAllocatorPageSize);
		resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		initial_resource_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}
	else if (allocation_type_ == linear_allocator_type::CpuWritable)
	{
		heap_pops.Type = D3D12_HEAP_TYPE_UPLOAD;
		resource_desc.Width = (page_size ? page_size : kCpuAllocatorPageSize);
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		initial_resource_state = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	else
	{
		initial_resource_state = D3D12_RESOURCE_STATE_COMMON;
		ASSERT(false);
	}

	ID3D12Resource* pBuffer = nullptr;
	ASSERT_SUCCEEDED(get_rhi()->device->CreateCommittedResource(&heap_pops, D3D12_HEAP_FLAG_NONE,
		&resource_desc, initial_resource_state, nullptr, IID_PPV_ARGS(&pBuffer)));

	pBuffer->SetName(L"linear_allocator_page_manager page");

	return new linear_allocation_page(pBuffer, initial_resource_state);
}

void linear_allocator_page_manager::discard_pages(uint64_t fence_value, std::vector<linear_allocation_page*> const& pages)
{
	std::lock_guard<std::mutex> guard(mutex_);

	for (linear_allocation_page* page : pages)
	{
		retired_pages_.push(std::make_pair(fence_value, page));
	}
}

void linear_allocator_page_manager::free_large_pages(uint64_t fence_value, std::vector<linear_allocation_page*> const& pages)
{
	std::lock_guard<std::mutex> guard(mutex_);

	//销毁已经使用完的页
	while (!deletion_queue_.empty() && get_rhi()->command_manager_.is_fence_complete(deletion_queue_.front().first))
	{
		delete deletion_queue_.front().second;
		deletion_queue_.pop();
	}

	for (linear_allocation_page* page : pages)
	{
		page->unmap();
		deletion_queue_.push(std::make_pair(fence_value, page));
	}
}

void linear_allocator_page_manager::destroy()
{
	page_pool_.clear();
}
