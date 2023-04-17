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
	//以不同的Flags创建DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
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

	if (dsv_handle_[0].ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		dsv_handle_[0] = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
	}

	//无标志
	dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
	get_rhi()->device->CreateDepthStencilView(rhi_resource_.Get(), &dsv_desc, dsv_handle_[0]);

	//只读深度
	dsv_desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
	get_rhi()->device->CreateDepthStencilView(rhi_resource_.Get(), &dsv_desc, dsv_handle_[1]);

	DXGI_FORMAT stencil_fmt = get_stencil_format(format);
	if (stencil_fmt != DXGI_FORMAT_UNKNOWN)
	{
		if (dsv_handle_[2].ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			dsv_handle_[2] = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
			dsv_handle_[3] = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
		}

		//只读模板
		dsv_desc.Flags = D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		get_rhi()->device->CreateDepthStencilView(rhi_resource_.Get(), &dsv_desc, dsv_handle_[2]);

		//只读深度+模板
		dsv_desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		get_rhi()->device->CreateDepthStencilView(rhi_resource_.Get(), &dsv_desc, dsv_handle_[3]);
	}
	else
	{
		dsv_handle_[2] = dsv_handle_[0];
		dsv_handle_[2] = dsv_handle_[1];
	}
	
	//为“深度”缓存创建SRV
	if (depth_srv_handle_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		depth_srv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = get_depth_format(format);
	if (dsv_desc.ViewDimension == D3D12_DSV_DIMENSION_TEXTURE2D)
	{
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = 1;
	}
	else
	{
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
	}
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	get_rhi()->device->CreateShaderResourceView(rhi_resource_.Get(), &srv_desc, depth_srv_handle_);

	//为“模版”缓存创建SRV
	if (stencil_fmt != DXGI_FORMAT_UNKNOWN)
	{
		if (stencil_srv_handle_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
		{
			stencil_srv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
		}
		srv_desc.Format = stencil_fmt;
		get_rhi()->device->CreateShaderResourceView(rhi_resource_.Get(), &srv_desc, stencil_srv_handle_);
	}
}
