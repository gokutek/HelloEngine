#pragma once

#include "pixel_buffer.h"
#include <string>

/*
===============================================================================
===============================================================================
*/
class depth_buffer : public pixel_buffer
{
public:
	depth_buffer(float clear_depth, uint8_t clear_stencil);

	float get_depth_clear_value() const;
	uint8_t get_stencil_clear_value() const;

	void create(std::wstring const& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);
	void create(std::wstring const& name, uint32_t width, uint32_t height, uint32_t samples_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);

	D3D12_CPU_DESCRIPTOR_HANDLE get_dsv() const;
	D3D12_CPU_DESCRIPTOR_HANDLE get_dsv_depth_readonly() const;
	D3D12_CPU_DESCRIPTOR_HANDLE get_dsv_stencil_readonly() const;
	D3D12_CPU_DESCRIPTOR_HANDLE get_dsv_readonly() const;
	D3D12_CPU_DESCRIPTOR_HANDLE get_depth_srv() const;
	D3D12_CPU_DESCRIPTOR_HANDLE get_stencil_srv() const;

protected:
	void create_derived_views(ID3D12Device* device, DXGI_FORMAT format);

private:
	float depth_clear_value_;
	uint8_t stencil_clear_value_;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle_[4];
	D3D12_CPU_DESCRIPTOR_HANDLE depth_srv_handle_;
	D3D12_CPU_DESCRIPTOR_HANDLE stencil_srv_handle_;
};

inline float depth_buffer::get_depth_clear_value() const
{
	return depth_clear_value_;
}

inline uint8_t depth_buffer::get_stencil_clear_value() const
{
	return stencil_clear_value_;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_dsv() const 
{
	return  dsv_handle_[0]; 
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_dsv_depth_readonly() const
{
	return  dsv_handle_[1];
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_dsv_stencil_readonly() const
{
	return  dsv_handle_[2];
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_dsv_readonly() const
{
	return  dsv_handle_[3];
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_depth_srv() const
{
	return depth_srv_handle_;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_stencil_srv() const
{
	return stencil_srv_handle_;
}
