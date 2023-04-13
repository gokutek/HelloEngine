#include "pixel_buffer.h"
#include "utility.h"

pixel_buffer::pixel_buffer() :
	width_(0),
	height_(0),
	array_size_(0),
	format_(DXGI_FORMAT_UNKNOWN)
{
}

void pixel_buffer::export_to_file(wchar_t const* fpath)
{
	ASSERT(false);
}

DXGI_FORMAT pixel_buffer::get_base_format(DXGI_FORMAT format)
{
    /*
    ===================================================
    1. 此函数是CTRL+CV过来的；
    2. 返回的格式都带`_TYPELESS`后缀；
    ===================================================
    */
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        return DXGI_FORMAT_R8G8B8A8_TYPELESS;

    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8A8_TYPELESS;

    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return DXGI_FORMAT_B8G8R8X8_TYPELESS;

        // 32-bit Z w/ Stencil
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_R32G8X24_TYPELESS;

        // No Stencil
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_R32_TYPELESS;

        // 24-bit Z
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_R24G8_TYPELESS;

        // 16-bit Z w/o Stencil
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        return DXGI_FORMAT_R16_TYPELESS;

    default:
        return format;
    }
}

D3D12_RESOURCE_DESC pixel_buffer::describe_tex_2d(uint32_t width, uint32_t height, uint16_t depth_or_array_size, uint32_t mip_level, DXGI_FORMAT format, uint32_t flags)
{
	//FIXME: 此处的设计貌似不好
	width_ = width;
	height_ = height;
	array_size_ = depth_or_array_size;
	format_ = format;

	D3D12_RESOURCE_DESC desc = {};
	desc.Alignment = 0;
	desc.DepthOrArraySize = depth_or_array_size;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Flags = (D3D12_RESOURCE_FLAGS)flags;
	desc.Format = get_base_format(format);
	desc.Width = width;
	desc.Height = height;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.MipLevels = mip_level;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	return desc;
}

void pixel_buffer::associate_with_resource(ID3D12Device* device, wchar_t const* name, ID3D12Resource* resource, D3D12_RESOURCE_STATES current_state)
{
    rhi_resource_.Attach(resource);
    usage_state_ = current_state;

    D3D12_RESOURCE_DESC desc = resource->GetDesc();
    width_ = (uint32_t)desc.Width; //FIXME: 改成uint64?
    height_ = desc.Height;
    array_size_ = desc.DepthOrArraySize;
    format_ = desc.Format;

#ifdef _DEBUG
    rhi_resource_->SetName(name);
#endif
}

void pixel_buffer::create_texture_resource(ID3D12Device* device, wchar_t const* name, const D3D12_RESOURCE_DESC& res_desc, D3D12_CLEAR_VALUE clear_value, D3D12_GPU_VIRTUAL_ADDRESS gpu_vaddr)
{
	destroy();

	CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
	ASSERT_SUCCEEDED(device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, &res_desc, D3D12_RESOURCE_STATE_COMMON, &clear_value, IID_PPV_ARGS(&rhi_resource_)));

	usage_state_ = D3D12_RESOURCE_STATE_COMMON;
	gpu_virtual_address_ = D3D12_GPU_VIRTUAL_ADDRESS_NULL;

#ifdef _DEBUG
	rhi_resource_->SetName(name);
#endif
}

DXGI_FORMAT pixel_buffer::get_dsv_format(DXGI_FORMAT format)
{
    switch (format)
    {
    // 32-bit Z w/ Stencil
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

    // No Stencil
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
        return DXGI_FORMAT_D32_FLOAT;

    // 24-bit Z
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        return DXGI_FORMAT_D24_UNORM_S8_UINT;

    // 16-bit Z w/o Stencil
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
        return DXGI_FORMAT_D16_UNORM;

    default:
        return format;
    }
}
