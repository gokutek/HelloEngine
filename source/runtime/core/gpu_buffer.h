#pragma once

#include "gpu_resource.h"

class upload_buffer;

/*
===============================================================================
≥ÈœÛ¿‡£ª
===============================================================================
*/
class gpu_buffer : public gpu_resource
{
public:
    // Create a buffer.  If initial data is provided, it will be copied into the buffer using the default command context.
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset = 0);

    // Sub-Allocate a buffer out of a pre-allocated heap.  If initial data is provided, it will be copied into the buffer using the default command context.
    int create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

protected:
    D3D12_RESOURCE_DESC describe_buffer() const;

    virtual void create_derived_views(void) = 0;

private:
    void create_buffer_and_view(wchar_t const* name, uint32_t num_elements, uint32_t element_size);

protected:
    uint32_t num_elements_;
    uint32_t element_size_;
    size_t buffer_size_;
    D3D12_RESOURCE_FLAGS resource_flags_;
};
