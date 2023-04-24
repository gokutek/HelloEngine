#include "byte_address_buffer.h"
#include "graphics_core.h"

void byte_address_buffer::create_derived_views()
{
    //创建SRV
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = (UINT)buffer_size_ / 4;
        desc.Buffer.StructureByteStride = 0;
        desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;

        if (srv_handle_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
        {
            srv_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
        }

        get_rhi()->device->CreateShaderResourceView(rhi_resource_.Get(), &desc, srv_handle_);
    }

    //创建UAV
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_R32_TYPELESS;
        desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = (UINT)buffer_size_ / 4;
        desc.Buffer.StructureByteStride = 0;
        desc.Buffer.CounterOffsetInBytes = 0;
        desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;

        if (uav_handle_.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
        {
            uav_handle_ = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
        }

        get_rhi()->device->CreateUnorderedAccessView(rhi_resource_.Get(), nullptr, &desc, uav_handle_);
    }
}
