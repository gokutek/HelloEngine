#include "gpu_buffer.h"
#include "graphics_core.h"

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
        assert(false && "not impl yet");
    }

    return 0;
}

int gpu_buffer::create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset)
{
    create_buffer_and_view(name, num_elements, element_size);

    // TODO: copy init data
    assert(false && "not impl yet");

    return 0;
}

int gpu_buffer::create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data)
{
    assert(false && "not impl yet");
    return -1;
}
