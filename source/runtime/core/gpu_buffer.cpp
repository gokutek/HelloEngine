﻿#include "gpu_buffer.h"
#include "graphics_core.h"
#include "command_context_manager.h"
#include "math/common.h"

gpu_buffer::gpu_buffer()
    : num_elements_(0),
    element_size_(0),
    buffer_size_(0),
    resource_flags_(D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
{
    uav_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    srv_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
}

D3D12_RESOURCE_DESC gpu_buffer::describe_buffer() const
{
    D3D12_RESOURCE_DESC resource_desc = {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Alignment = 0;
    resource_desc.Width = buffer_size_;
    resource_desc.Height = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.MipLevels = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.SampleDesc.Quality = 0;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resource_desc.Flags = resource_flags_;
    return resource_desc;
}

void gpu_buffer::create_buffer_and_view(wchar_t const* name, uint32_t num_elements, uint32_t element_size)
{
    destroy();

    num_elements_ = num_elements;
    element_size_ = element_size;
    buffer_size_ = num_elements * element_size;

    D3D12_RESOURCE_DESC resource_desc = describe_buffer();

    D3D12_HEAP_PROPERTIES heap_prop = {};
    heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;
    heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heap_prop.CreationNodeMask = 0;
    heap_prop.VisibleNodeMask = 0;

    usage_state_ = D3D12_RESOURCE_STATE_COMMON;

    ASSERT_SUCCEEDED(get_rhi()->device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc, usage_state_, nullptr, IID_PPV_ARGS(&rhi_resource_)));
    rhi_resource_->SetName(name);

    gpu_virtual_address_ = rhi_resource_->GetGPUVirtualAddress();

    create_derived_views();
}

int gpu_buffer::create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const void* initial_data)
{
    create_buffer_and_view(name, num_elements, element_size);

    if (initial_data)
    {
        command_context* context = get_rhi()->context_manager_.allocate_graphics_context(L"gpu_buffer::create");
        context->initialize_buffer(*this, initial_data, buffer_size_);
    }

    return 0;
}

int gpu_buffer::create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset)
{
    create_buffer_and_view(name, num_elements, element_size);

    graphics_context* context = get_rhi()->context_manager_.allocate_graphics_context(L"gpu_buffer::create");
    context->initialize_buffer(*this, src_data, src_offset);
    context->finish(true);

    return 0;
}

int gpu_buffer::create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data)
{
    assert(false && "not impl yet");
    return -1;
}

D3D12_GPU_VIRTUAL_ADDRESS gpu_buffer::root_constant_buffer_view(void) const
{
    return gpu_virtual_address_;
}

D3D12_CPU_DESCRIPTOR_HANDLE gpu_buffer::create_constant_buffer_view(uint32_t offset, uint32_t size) const
{
    size = align_up(size, 16);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc;
    cbv_desc.BufferLocation = gpu_virtual_address_ + offset;
    cbv_desc.SizeInBytes = size;

    D3D12_CPU_DESCRIPTOR_HANDLE handle = get_rhi()->allocate_descriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
    get_rhi()->device->CreateConstantBufferView(&cbv_desc, handle);
    return handle;
}

D3D12_VERTEX_BUFFER_VIEW gpu_buffer::vertex_buffer_view(size_t offset, uint32_t size, uint32_t stride) const
{
    D3D12_VERTEX_BUFFER_VIEW view;
    view.BufferLocation = gpu_virtual_address_ + offset;
    view.SizeInBytes = size;
    view.StrideInBytes = stride;
    return view;
}

D3D12_INDEX_BUFFER_VIEW gpu_buffer::index_buffer_view(size_t offset, uint32_t size, bool b32bit) const
{
    D3D12_INDEX_BUFFER_VIEW view;
    view.BufferLocation = gpu_virtual_address_ + offset;
    view.Format = b32bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
    view.SizeInBytes = size;
    return view;
}
