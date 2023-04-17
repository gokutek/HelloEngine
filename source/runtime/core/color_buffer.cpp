#include "color_buffer.h"
#include "graphics_core.h"
#include "game_core.h"
#include "utility.h"

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
	rtv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
	get_rhi()->device->CreateRenderTargetView(rhi_resource_.Get(), nullptr, rtv_handle_);
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
    ASSERT(array_size == 1 || mip_level == 1, "We don't support auto-mips on texture arrays");

    mips_level_ = mip_level - 1;

    D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

    rtv_desc.Format = format;
    uav_desc.Format = get_uav_format(format);
    srv_desc.Format = format;
    srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (array_size > 1)
    {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
        rtv_desc.Texture2DArray.MipSlice = 0;
        rtv_desc.Texture2DArray.FirstArraySlice = 0;
        rtv_desc.Texture2DArray.ArraySize = (UINT)array_size;

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
        uav_desc.Texture2DArray.MipSlice = 0;
        uav_desc.Texture2DArray.FirstArraySlice = 0;
        uav_desc.Texture2DArray.ArraySize = (UINT)array_size;

        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srv_desc.Texture2DArray.MipLevels = mip_level;
        srv_desc.Texture2DArray.MostDetailedMip = 0;
        srv_desc.Texture2DArray.FirstArraySlice = 0;
        srv_desc.Texture2DArray.ArraySize = (UINT)array_size;
    }
    else if (fragment_count_ > 1)
    {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
    }
    else
    {
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtv_desc.Texture2D.MipSlice = 0;

        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uav_desc.Texture2D.MipSlice = 0;

        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = mip_level;
        srv_desc.Texture2D.MostDetailedMip = 0;
    }

    if (srv_handle_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
    {
        rtv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
        srv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
    }

    ID3D12Resource* Resource = rhi_resource_.Get();

    // Create the render target view
    get_rhi()->device->CreateRenderTargetView(Resource, &rtv_desc, rtv_handle_);

    // Create the shader resource view
    get_rhi()->device->CreateShaderResourceView(Resource, &srv_desc, srv_handle_);

    if (fragment_count_ > 1)
    {
        return;
    }

    // Create the UAVs for each mip level (RWTexture2D)
    for (uint32_t i = 0; i < mip_level; ++i)
    {
        if (uav_handle_[i].ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
        {
            uav_handle_[i] = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
        }

        get_rhi()->device->CreateUnorderedAccessView(Resource, nullptr, &uav_desc, uav_handle_[i]);

        uav_desc.Texture2D.MipSlice++;
    }
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
