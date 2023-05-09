#pragma once

#include "root_signature.h"
#include "dynamic_descriptor_heap_inner.h"
#include <vector>
#include <mutex>
#include <queue>

class command_context;

/**
 * @brief TODO:
 */
class dynamic_descriptor_heap
{
public:
	/**
	 * @brief 
	 * @param owning_context 
	 * @param heap_type 
	 */
	dynamic_descriptor_heap(command_context* owning_context, D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

	/**
	 * @brief 
	 */
	~dynamic_descriptor_heap();

	/**
	 * @brief 
	 */
	void cleanup_used_heaps(uint64_t fence_value);

	/**
	 * @brief Copy multiple handles into the cache area reserved for the specified root parameter.
	 * @param root_index 
	 * @param offset 
	 * @param count 
	 * @param handles 
	 */
	void set_graphics_descriptor_handles(uint32_t root_index, uint32_t offset, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles);
	void set_compute_descriptor_handles(uint32_t root_index, uint32_t offset, uint32_t count,  D3D12_CPU_DESCRIPTOR_HANDLE const* handles);

    /**
     * @brief Bypass the cache and upload directly to the shader-visible heap
     * @param handles 
     * @return 
     */
    D3D12_GPU_DESCRIPTOR_HANDLE upload_direct(D3D12_CPU_DESCRIPTOR_HANDLE handles);

    /**
     * @brief Deduce cache layout needed to support the descriptor tables needed by the root signature.
     * @param root_sig 
     */
    void parse_graphics_root_signature(root_signature const& root_sig);

    void parse_compute_root_signature(root_signature const& root_sig);

    /**
     * @brief Upload any new descriptors in the cache to the shader-visible heap.
     * @param cmd_list 
     */
    void commit_graphics_root_descriptor_tables(ID3D12GraphicsCommandList* cmd_list);

    void commit_compute_root_descriptor_tables(ID3D12GraphicsCommandList* cmd_list);

private:
    /**
     * @brief 获取一个DescriptorHeap，优先返回可复用的，否则创建新的。
     * @param heap_type 
     * @return 
     */
    ID3D12DescriptorHeap* request_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

    /**
     * @brief 对应request_descriptor_heap，用完后放入回收池。
     * @param heap_type 
     * @param fence_value_for_reset 
     * @param used_heaps 
     */
    void discard_descriptor_heaps(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, uint64_t fence_value_for_reset, std::vector<ID3D12DescriptorHeap*> const& used_heaps);

	/**
	 * @brief 当前堆中是否还有count个可用的descriptor空间
	 * @param count 
	 * @return 
	 */
	bool has_space(uint32_t count) const;

    /**
     * @brief 封存当前堆
     */
    void retire_current_heap();

    /**
     * @brief 
     * @param fence_value 
     */
    void retire_used_heaps(uint64_t fence_value);

    /**
     * @brief 获取当前堆的指针，若当前堆为null则会触发分配
     * @return 
     */
    ID3D12DescriptorHeap* get_heap_pointer();

    /**
     * @brief 在当前堆里分配count个Descriptor的空间
     * @param count 
     * @return 
     */
    descriptor_handle allocate(uint32_t count);

    /**
     * @brief 
     * @param handle_cache 
     * @param cmd_list 
     * @param setter 
     */
    void copy_and_bind_staged_tables(descriptor_handle_cache& handle_cache, ID3D12GraphicsCommandList* cmd_list,
        void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::*setter)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

    /**
     * @brief Mark all descriptors in the cache as stale and in need of re-uploading.
     * @param  
     */
    void unbind_all_valid();

private:
    static constexpr uint32_t kNumDescriptorsPerHeap = 1024;
    std::mutex mutex_;
    std::vector<ComPtr<ID3D12DescriptorHeap> > descriptor_heap_pool_[2]; ///< 所有创建的
    std::queue<std::pair<uint64_t, ID3D12DescriptorHeap*> > retired_descriptor_heaps_[2]; ///< 已释放但GPU可能还在用
    std::queue<ID3D12DescriptorHeap*> available_descriptor_heaps_[2]; ///< 可用的

    command_context* owning_context_;
    D3D12_DESCRIPTOR_HEAP_TYPE const descriptor_heap_type_;
    uint32_t const descriptor_size_;
    ID3D12DescriptorHeap* current_heap_; ///< 当前分配的堆
    uint32_t current_offset_; ///< 当前堆已被分配的元素个数
    descriptor_handle first_descriptor_handle_; ///< 当前堆的首元素句柄
    std::vector<ID3D12DescriptorHeap*> retired_heaps_; ///< 已使用的DescriptorHeap
};
