#include "depth_buffer.h"
#include "graphics_core.h"

depth_buffer::depth_buffer(float clear_depth, uint8_t clear_stencil)
{
	depth_clear_value_ = clear_depth;
	stencil_clear_value_ = clear_stencil;

	dsv_handle_[0].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	dsv_handle_[1].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	dsv_handle_[2].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	dsv_handle_[3].ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	depth_srv_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	stencil_srv_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
}

void depth_buffer::create(std::wstring const& name, uint32_t width, uint32_t height, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	create(name, width, height, 1, format, gpu_va);
}

void depth_buffer::create(std::wstring const& name, uint32_t width, uint32_t height, uint32_t samples_count, DXGI_FORMAT format, D3D12_GPU_VIRTUAL_ADDRESS gpu_va)
{
	D3D12_RESOURCE_DESC desc = describe_tex_2d(width, height, 1, 1, format, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	desc.SampleDesc.Count = samples_count;

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	create_texture_resource(get_rhi()->device, name.c_str(), desc, clear_value, gpu_va);

	create_derived_views(get_rhi()->device, format);
}

void depth_buffer::create_derived_views(ID3D12Device* device, DXGI_FORMAT format)
{
	//创建DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Format = get_dsv_format(format);
	if (rhi_resource_->GetDesc().SampleDesc.Count == 1)
	{
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;
	}
	else
	{
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	}

	typedef struct D3D12_DEPTH_STENCIL_VIEW_DESC
	{
		DXGI_FORMAT Format;
		D3D12_DSV_DIMENSION ViewDimension;
		D3D12_DSV_FLAGS Flags;
		union
		{
			D3D12_TEX1D_DSV Texture1D;
			D3D12_TEX1D_ARRAY_DSV Texture1DArray;
			D3D12_TEX2D_DSV Texture2D;
			D3D12_TEX2D_ARRAY_DSV Texture2DArray;
			D3D12_TEX2DMS_DSV Texture2DMS;
			D3D12_TEX2DMS_ARRAY_DSV Texture2DMSArray;
		};
	} 	D3D12_DEPTH_STENCIL_VIEW_DESC;




	//创建SRV
}

D3D12_CPU_DESCRIPTOR_HANDLE depth_buffer::get_dsv() const
{
	//TODO:
	D3D12_CPU_DESCRIPTOR_HANDLE handle{};
	return handle;
}
