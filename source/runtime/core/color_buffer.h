#pragma once

#include "pixel_buffer.h"
#include "color.h"

class color_buffer : public pixel_buffer
{
public:
	color_buffer(color clear_color);

	void create_from_swap_chain(wchar_t const* name, ID3D12Resource* resource);
	void create(wchar_t const* name, uint32_t width, uint32_t height, uint32_t mip_level, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va);

	const D3D12_CPU_DESCRIPTOR_HANDLE& get_srv() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& get_rtv() const;
	const D3D12_CPU_DESCRIPTOR_HANDLE& get_uav() const;

protected:
	D3D12_RESOURCE_FLAGS combine_resource_flags();
	static uint32_t compute_mips_level(uint32_t width, uint32_t height);
	/** 创建资源相关的VIEW，如RTV、SRV、UAV */
	void create_derived_views(ID3D12Device* device, DXGI_FORMAT format, uint32_t array_size, uint32_t mip_level);

protected:
	color clear_color_;
	D3D12_CPU_DESCRIPTOR_HANDLE srv_handle_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle_;
	uint32_t mips_level_;
	uint32_t fragment_count_;
	uint32_t sample_count_;
};

inline const D3D12_CPU_DESCRIPTOR_HANDLE& color_buffer::get_srv() const
{
	return srv_handle_;
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE& color_buffer::get_rtv() const
{
	return rtv_handle_;
}

inline const D3D12_CPU_DESCRIPTOR_HANDLE& color_buffer::get_uav() const
{
	//TODO:
	return srv_handle_;
}
