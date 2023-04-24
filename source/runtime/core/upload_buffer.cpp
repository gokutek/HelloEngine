#include "upload_buffer.h"
#include "graphics_core.h"

upload_buffer::upload_buffer()
	: buffer_size_(0)
{
}

int upload_buffer::create(wchar_t const* name, size_t buffer_size)
{
	destroy();

	buffer_size_ = buffer_size;
	
	D3D12_HEAP_PROPERTIES heap_prop = {};
	heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_prop.CreationNodeMask = 0;
	heap_prop.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resource_desc = {};
	resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resource_desc.Alignment = 0;
	resource_desc.Width = buffer_size;
	resource_desc.Height = 1;
	resource_desc.DepthOrArraySize = 1;
	resource_desc.MipLevels = 1;
	resource_desc.Format = DXGI_FORMAT_UNKNOWN;
	resource_desc.SampleDesc.Count = 1;
	resource_desc.SampleDesc.Quality = 0;
	resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ASSERT_SUCCEEDED(get_rhi()->device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&rhi_resource_)));
	rhi_resource_->SetName(name);

	gpu_virtual_address_ = rhi_resource_->GetGPUVirtualAddress();

	return 0;
}

void* upload_buffer::map()
{
	void* buffer = nullptr;
	rhi_resource_->Map(0, &CD3DX12_RANGE(0, buffer_size_), &buffer);
	return buffer;
}

void upload_buffer::unmap(size_t begin, size_t end)
{
	rhi_resource_->Unmap(0, &CD3DX12_RANGE(begin, std::min(end, buffer_size_)));
}
