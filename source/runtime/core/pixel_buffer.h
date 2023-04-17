#pragma once

#include "gpu_resource.h"

class pixel_buffer : public gpu_resource
{
public:
	pixel_buffer();

	uint32_t get_width() const;
	uint32_t get_height() const;
	uint32_t get_depth() const;
	DXGI_FORMAT get_format() const;

	void export_to_file(wchar_t const* fpath);

protected:
	static DXGI_FORMAT get_base_format(DXGI_FORMAT format);

	//TODO:
	D3D12_RESOURCE_DESC describe_tex_2d(uint32_t width, uint32_t height, uint16_t depth_or_array_size, uint32_t mip_level, DXGI_FORMAT format, uint32_t flags);
	void associate_with_resource(ID3D12Device* device, wchar_t const* name, ID3D12Resource* resource, D3D12_RESOURCE_STATES current_state);
	void create_texture_resource(ID3D12Device* device, wchar_t const* name, const D3D12_RESOURCE_DESC& res_desc, D3D12_CLEAR_VALUE clear_value, D3D12_GPU_VIRTUAL_ADDRESS gpu_vaddr);

	static DXGI_FORMAT get_dsv_format(DXGI_FORMAT format);
	static DXGI_FORMAT get_stencil_format(DXGI_FORMAT format);
	static DXGI_FORMAT get_depth_format(DXGI_FORMAT format);

protected:
	uint32_t width_;
	uint32_t height_;
	uint16_t array_size_;
	DXGI_FORMAT format_;
};

inline uint32_t pixel_buffer::get_width() const
{
	return width_;
}

inline uint32_t pixel_buffer::get_height() const
{
	return height_;
}

inline uint32_t pixel_buffer::get_depth() const
{
	return array_size_;
}

inline DXGI_FORMAT pixel_buffer::get_format() const
{
	return format_;
}
