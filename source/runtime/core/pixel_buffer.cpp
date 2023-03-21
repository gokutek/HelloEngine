#include "pixel_buffer.h"
#include "utility.h"

pixel_buffer::pixel_buffer() :
	width_(0),
	height_(0),
	array_size_(0),
	format_(DXGI_FORMAT_UNKNOWN)
{
}

void pixel_buffer::export_to_file(std::wstring const& fpath)
{
	ASSERT(false);
}

void pixel_buffer::create_texture_resource(ID3D12Device* device, const wchar_t* name, const D3D12_RESOURCE_DESC& res_desc, D3D12_CLEAR_VALUE clear_value, D3D12_GPU_VIRTUAL_ADDRESS gpu_vaddr)
{
	destroy();

	CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
	ASSERT_SUCCEEDED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&rhi_resource_)));

	usage_state_ = D3D12_RESOURCE_STATE_COMMON;
	gpu_virtual_address_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

#ifdef _DEBUG
	rhi_resource_->SetName(name);
#else
	name;
#endif
}
