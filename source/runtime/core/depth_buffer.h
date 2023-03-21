#pragma once

#include "pixel_buffer.h"
#include <string>

class depth_buffer : public pixel_buffer
{
public:
	depth_buffer(float clear_depth, uint8_t clear_stencil);

	void create(std::wstring const& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);
	void create(std::wstring const& name, uint32_t width, uint32_t height, uint32_t samples_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);

protected:
	void create_derived_views(ID3D12Device* device, DXGI_FORMAT format);
};
