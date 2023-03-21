#include "depth_buffer.h"

depth_buffer::depth_buffer(float clear_depth, uint8_t clear_stencil)
{
	//TODO:
}

void depth_buffer::create(std::wstring const& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	create(name, width, height, 1, format, gpu_va);
}

void depth_buffer::create(std::wstring const& name, uint32_t width, uint32_t height, uint32_t samples_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	//TODO:
}

void depth_buffer::create_derived_views(ID3D12Device* device, DXGI_FORMAT format)
{
	//TODO:
}
