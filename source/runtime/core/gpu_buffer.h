#pragma once

#include "pch.h"
#include "gpu_resource.h"

class upload_buffer;

/**
 * @brief 本类是一个抽象类
 */
class gpu_buffer : public gpu_resource
{
public:
    gpu_buffer();

    /**
     * @brief 创建Buffer
     * @param name
     * @param num_elements 元素个数
     * @param element_size 每个元素的大小
     * @param initial_data 初始化数据，若非空，则将拷贝过去 If initial data is provided, it will be copied into the buffer using the default command context.
     */
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

    /**
     * @brief 创建Buffer
     * @param name
     * @param num_elements
     * @param element_size
     * @param src_data
     * @param src_offset
     */
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset = 0);

    /**
     * @brief Sub-Allocate a buffer out of a pre-allocated heap. If initial data is provided, it will be copied into the buffer using the default command context.
     */
    int create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

    D3D12_CPU_DESCRIPTOR_HANDLE get_uav() const;
    D3D12_CPU_DESCRIPTOR_HANDLE get_srv() const;

    /**
     * @brief
     */
    D3D12_GPU_VIRTUAL_ADDRESS root_constant_buffer_view(void) const;

    /**
     * @brief
     */
    D3D12_CPU_DESCRIPTOR_HANDLE create_constant_buffer_view(uint32_t offset, uint32_t size) const;

    /**
     * @brief
     */
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view(size_t offset, uint32_t size, uint32_t stride) const;

    /**
     * @brief
     */
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view(size_t base_vertex_index = 0) const;

    /**
     * @brief
     */
    D3D12_INDEX_BUFFER_VIEW index_buffer_view(size_t offset, uint32_t size, bool b32bit = false) const;

    /**
     * @brief
     */
    D3D12_INDEX_BUFFER_VIEW index_buffer_view(size_t start_index = 0) const;

    size_t get_buffer_size() const;
    uint32_t get_element_count() const;
    uint32_t get_element_size() const;
                                   
protected:
    D3D12_RESOURCE_DESC describe_buffer() const;

    /** 
     * @brief 创建资源相关的View，由子类实现 
     */
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
