#include "color_buffer.h"
#include "graphics_core.h"
#include "game_core.h"

color_buffer::color_buffer() : color_buffer(color(0, 0, 0, 0))
{
}

color_buffer::color_buffer(color const& clear_color) :
	clear_color_(clear_color),
	mips_level_(0),
	fragment_count_(1),
	sample_count_(1)
{
	srv_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	rtv_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	for (int i = 0; i < _countof(uav_handle_); ++i)
	{
		uav_handle_[i].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	}
}

void color_buffer::create_from_swap_chain(wchar_t const* name, ID3D12Resource* resource)
{
	associate_with_resource(get_rhi()->device, name, resource, D3D12_RESOURCE_STATE_PREDICATION);
	//rtv_handle_ = 
	//TODO:
}

void color_buffer::create(wchar_t const* name, uint32_t width, uint32_t height, uint32_t mip_level, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	mip_level = mip_level > 0 ? mip_level : compute_mips_level(width, height);

	D3D12_RESOURCE_FLAGS flags = combine_resource_flags();
	
	D3D12_RESOURCE_DESC desc = describe_tex_2d(width, height, 1, mip_level, format, flags);
	desc.SampleDesc.Count = fragment_count_;
	desc.SampleDesc.Quality = 0;
	
	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	clear_value.Color[0] = clear_color_.get_r();
	clear_value.Color[1] = clear_color_.get_g();
	clear_value.Color[2] = clear_color_.get_b();
	clear_value.Color[3] = clear_color_.get_a();

	create_texture_resource(get_rhi()->device, name, desc, clear_value, gpu_va);
	create_derived_views(get_rhi()->device, format, 1, mip_level);
}

void color_buffer::create_array(wchar_t const* name, uint32_t width, uint32_t height, uint32_t array_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	D3D12_RESOURCE_FLAGS flags = combine_resource_flags();

	D3D12_RESOURCE_DESC desc = describe_tex_2d(width, height, array_count, 1, format, flags);

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	clear_value.Color[0] = clear_color_.get_r();
	clear_value.Color[1] = clear_color_.get_g();
	clear_value.Color[2] = clear_color_.get_b();
	clear_value.Color[3] = clear_color_.get_a();

	create_texture_resource(get_rhi()->device, name, desc, clear_value, gpu_va);
	create_derived_views(get_rhi()->device, format, array_count, 1);
}

void color_buffer::create_derived_views(ID3D12Device* device, DXGI_FORMAT format, uint32_t array_size, uint32_t mip_level)
{
	//TODO:

	D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
	D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

	rtv_desc.Format = format;
	uav_desc.Format; //TODO:
	srv_desc.Format = format;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

}

D3D12_RESOURCE_FLAGS color_buffer::combine_resource_flags()
{
	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;
	if (fragment_count_ == 1)
	{
		flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	return flags;
}

uint32_t color_buffer::compute_mips_level(uint32_t width, uint32_t height)
{
	//TODO: 理解本算法
	unsigned long HighBit;
	_BitScanReverse((unsigned long*)&HighBit, width | height);
	return HighBit + 1;
}
