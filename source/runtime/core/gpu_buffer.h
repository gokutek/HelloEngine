#pragma once

#include "pch.h"
#include "gpu_resource.h"

class upload_buffer;

/**
 * @brief 本类是一个抽象类。
 * 
 * buffer即GPU上分配的内存，可以存储顶点数据、顶点索引、shader中要用的constant buffer里的参数等等。
 * 这些数据可以都存在一个Buffer里(逻辑上分成几块)，也可以存在多个Buffer里。
 * D3D12_VERTEX_BUFFER_VIEW、D3D12_INDEX_BUFFER_VIEW会指示访问该buffer中的哪块数据。
 */
class gpu_buffer : public gpu_resource
{
public:
    gpu_buffer();

    /**
     * @brief 创建Buffer，buffer的大小由num_elements和element_size两个参数决定
     * @param name
     * @param num_elements 元素个数
     * @param element_size 每个元素的大小
     * @param initial_data 初始化数据，若非空，则将拷贝过去 If initial data is provided, it will be copied into the buffer using the default command context.
     */
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

    /**
     * @brief 创建Buffer
     * @param name
     * @param num_elements 元素个数
     * @param element_size 每个元素的大小
     * @param src_data
     * @param src_offset
     */
    int create(wchar_t const* name, uint32_t num_elements, uint32_t element_size, const upload_buffer& src_data, uint32_t src_offset = 0);

    /**
     * @brief Sub-Allocate a buffer out of a pre-allocated heap. If initial data is provided, it will be copied into the buffer using the default command context.
     */
    int create(wchar_t const* name, ID3D12Heap* backing_heap, uint32_t heap_offset, uint32_t num_elements, uint32_t element_size, const void* initial_data = nullptr);

    /**
     * @brief 
     * @return 
     */
    D3D12_CPU_DESCRIPTOR_HANDLE get_uav() const;

    /**
     * @brief 
     * @return 
     */
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
     * @brief 将该块buffer作为Vertex Buffer，返回对应的View
     * @param offset 这块buffer在整个buffer中的偏移
     * @param size 长度
     * @param stride 步幅，即每个顶点的字节长度
     * @return 
     */
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view(size_t offset, uint32_t size, uint32_t stride) const;

    /**
     * @brief 对应某块buffer的view，这个接口适合于整个buffer存储的是顶点数组的情况
     * @param base_vertex_index 
     * @return 
     */
    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view(size_t base_vertex_index = 0) const;

    /**
     * @brief 将该块buffer作为Index Buffer，返回对应的View
     * @param offset 
     * @param size 
     * @param b32bit 索引是否为32位（否则为16位）
     * @return 
     */
    D3D12_INDEX_BUFFER_VIEW index_buffer_view(size_t offset, uint32_t size, bool b32bit = false) const;

    /**
     * @brief 
     * @param start_index 
     * @return 
     */
    D3D12_INDEX_BUFFER_VIEW index_buffer_view(size_t start_index = 0) const;

    /**
     * @brief 整块buffer的内存大小
     * @return 
     */
    size_t get_buffer_size() const;
    
    /**
     * @brief 元素个数
     * @return 
     */
    uint32_t get_element_count() const;

    /**
     * @brief 每个元素的大小
     * @return 
     */
    uint32_t get_element_size() const;
                                   
protected:
    /**
     * @brief 
     * @return 
     */
    D3D12_RESOURCE_DESC describe_buffer() const;

    /** 
     * @brief 创建资源相关的View，由子类实现 
     */
    virtual void create_derived_views() = 0;

private:
    /**
     * @brief 
     * @param name 
     * @param num_elements 
     * @param element_size 
     */
    void create_buffer_and_view(wchar_t const* name, uint32_t num_elements, uint32_t element_size);

protected:
    uint32_t num_elements_; ///< 元素个数
    uint32_t element_size_; ///< 每个元素的大小
    size_t buffer_size_; ///< 缓冲区大小，即上面两个属性的乘积
    D3D12_RESOURCE_FLAGS resource_flags_; ///<

    D3D12_CPU_DESCRIPTOR_HANDLE uav_handle_; ///<
    D3D12_CPU_DESCRIPTOR_HANDLE srv_handle_; ///<
};

inline D3D12_CPU_DESCRIPTOR_HANDLE gpu_buffer::get_uav() const
{
    return uav_handle_;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE gpu_buffer::get_srv() const
{
    return srv_handle_;
}

inline size_t gpu_buffer::get_buffer_size() const
{
    return buffer_size_;
}

inline uint32_t gpu_buffer::get_element_count() const
{
    return num_elements_;
}

inline uint32_t gpu_buffer::get_element_size() const
{
    return element_size_;
}

inline D3D12_VERTEX_BUFFER_VIEW gpu_buffer::vertex_buffer_view(size_t base_vertex_index) const
{
    size_t offset = base_vertex_index * element_size_;
    return vertex_buffer_view(offset, (uint32_t)(buffer_size_ - offset), element_size_);
}

inline D3D12_INDEX_BUFFER_VIEW gpu_buffer::index_buffer_view(size_t start_index) const
{
    size_t offset = start_index * element_size_;
    return index_buffer_view(offset, (uint32_t)(buffer_size_ - offset), element_size_ == 4);
}
