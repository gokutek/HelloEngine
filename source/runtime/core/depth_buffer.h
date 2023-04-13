#pragma once

#include "pixel_buffer.h"
#include <string>

class depth_buffer : public pixel_buffer
{
public:
	depth_buffer(float clear_depth, uint8_t clear_stencil);

	void create(std::wstring const& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);
	void create(std::wstring const& name, uint32_t width, uint32_t height, uint32_t samples_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);

	D3D12_CPU_DESCRIPTOR_HANDLE get_dsv() const;

protected:
	void create_derived_views(ID3D12Device* device, DXGI_FORMAT format);

private:
	float depth_clear_value_;
	uint8_t stencil_clear_value_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle_[4];
	D3D12_CPU_DESCRIPTOR_HANDLE depth_srv_handle_;
	D3D12_CPU_DESCRIPTOR_HANDLE stencil_srv_handle_;
};
