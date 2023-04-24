#pragma once

#include "pch.h"
#include "gpu_resource.h"

class upload_buffer;

/*
===============================================================================
抽象类；
===============================================================================
*/
class gpu_buffer : public gpu_resource
{
public:
    gpu_buffer();

    // Create a buffer.  If initial data is provided, it will be copied into the buffer using the default command context.
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset = 0);

    // Sub-Allocate a buffer out of a pre-allocated heap.  If initial data is provided, it will be copied into the buffer using the default command context.
    int create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE get_uav() const;
    D3D12_CPU_DESCRIPTOR_HANDLE get_srv() const;
                                   
protected:
    D3D12_RESOURCE_DESC describe_buffer() const;

    /** 子类需实现该接口来创建Buffer对应的View */
    virtual void create_derived_views() = 0;

private:
    void create_buffer_and_view(wchar_t const* name, uint32_t num_elements, uint32_t element_size);

protected:
    uint32_t num_elements_;
    uint32_t element_size_;
    size_t buffer_size_;
    D3D12_RESOURCE_FLAGS resource_flags_;

    D3D12_CPU_DESCRIPTOR_HANDLE uav_handle_;
    D3D12_CPU_DESCRIPTOR_HANDLE srv_handle_;
};

inline D3D12_CPU_DESCRIPTOR_HANDLE gpu_buffer::get_uav() const
{
    return uav_handle_;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE gpu_buffer::get_srv() const
{
    return srv_handle_;
}
