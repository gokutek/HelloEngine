#pragma once

#include "pch.h"
#include "gpu_resource.h"
#include "gpu_buffer.h"
#include "dynamic_descriptor_heap.h"
#include "linear_allocator.h"
#include "upload_buffer.h"
#include "pso.h"
#include <stdint.h>
#include <string>

class command_list_manager;

/**
 * @brief 在每一帧记录渲染命令时，都需要一个command_context。
 * 
 * 本类中的大部分接口都只是生成一条命令，而不是立即执行。
 * 
 * 子类有：
 * @see graphics_context
 * @see compute_context
 */
class command_context
{
public:
	/**
	 * @brief 
	 * @param type 
	 */
	command_context(D3D12_COMMAND_LIST_TYPE type);

	/**
	 * @brief 
	 */
	virtual ~command_context();

	/**
	 * @brief 初始化
	 */
	void initialize();

	/**
	 * @brief 
	 * @param wait_for_completion 
	 * @return 
	 */
	uint64_t flush(bool wait_for_completion);

	/**
	 * @brief 
	 * @param wait_for_completion 是否要等待GPU执行完成
	 * @return 
	 */
	uint64_t finish(bool wait_for_completion);

	/**
	 * @brief 
	 */
	void reset();

	/**
	 * @brief 
	 * @return 
	 */
	ID3D12GraphicsCommandList* get_command_list();

	/**
	 * @brief 该操作会改变dst和src的资源屏障属性
	 * @param dst 
	 * @param src 
	 * 
	 * @see copy_buffer_region
	 */
	void copy_buffer(gpu_resource& dst, gpu_resource& src);

	/**
	 * @brief 拷贝部分数据
	 * @param dest 
	 * @param dest_offset 
	 * @param src 
	 * @param src_offset 
	 * @param num_bytes 
	 * 
	 * @see copy_buffer
	 */
	void copy_buffer_region(gpu_resource& dest, size_t dest_offset, gpu_resource& src, size_t src_offset, size_t num_bytes);

	/**
	 * @brief 即CopyTextureRegion
	 * @param dest 
	 * @param dest_sub_index 
	 * @param src 
	 * @param src_sub_index 
	 */
	void copy_sub_resource(gpu_resource& dest, UINT dest_sub_index, gpu_resource& src, UINT src_sub_index);

	//void copy_counter(gpu_resource& dest, size_t dest_offset, StructuredBuffer& src);

	/**
	 * @brief 
	 * @param dest 
	 * @param x 
	 * @param y 
	 * @param z 
	 * @param source 
	 * @param rect 
	 */
	void copy_texture_region(gpu_resource& dest, UINT x, UINT y, UINT z, gpu_resource& source, RECT& rect);

	//void reset_counter(StructuredBuffer& buf, uint32_t value = 0);
	//uint32_t ReadbackTexture(ReadbackBuffer& DstBuffer, PixelBuffer& SrcBuffer);

	/**
	 * @brief 
	 * @param num_bytes 
	 * @return 
	 */
	dyn_alloc reserve_upload_memory(size_t num_bytes);

	/**
	 * @brief 
	 * @param dest 
	 * @param num_sub_resources 
	 * @param subdata 
	 */
	void initialize_texture(gpu_resource& dest, UINT num_sub_resources, D3D12_SUBRESOURCE_DATA subdata[]);

	/**
	 * @brief 以CPU内存来初始化（从CPU到GPU，可能需要引入一个中间Buffer）
	 * @param dest GPU目的地址
	 * @param data CPU源地址
	 * @param num_bytes 拷贝的字节数
	 * @param dest_offset 目的地址偏移值
	 */
	void initialize_buffer(gpu_buffer& dest, const void* data, size_t num_bytes, size_t dest_offset = 0);

	/**
	 * @brief 以另一个Buffer的数据来初始化（两个Buffer都是GPU内存）
	 * @param dest GPU目的地址
	 * @param src GPU源地址
	 * @param src_offset 源地址偏移值
	 * @param num_bytes 拷贝的字节数
	 * @param dest_offset 目的地址偏移值 
	 */
	void initialize_buffer(gpu_buffer& dest, const upload_buffer& src, size_t src_offset, size_t num_bytes = -1, size_t dest_offset = 0);

	/**
	 * @brief 
	 * @param dest 
	 * @param slice_index 
	 * @param src 
	 */
	void initialize_texture_array_slice(gpu_resource& dest, UINT slice_index, gpu_resource& src);

	/**
	 * @brief 
	 * @param dest 
	 * @param dest_offset 
	 * @param data 
	 * @param num_bytes 
	 */
	void write_buffer(gpu_resource& dest, size_t dest_offset, const void* data, size_t num_bytes);

	//void fill_buffer(gpu_resource& dest, size_t dest_offset, DWParam Value, size_t num_bytes);

	/**
	 * @brief 资源屏障
	 * @param resource 资源
	 * @param new_state 资源新的状态
	 * @param flush_immediate 是否立即提交到命令列表
	 */
	void transition_resource(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate);

	void begin_resource_transition(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate = false);
	void insert_uav_barrier(gpu_resource& resource, bool flush_immediate = false);
	void insert_alias_barrier(gpu_resource& before, gpu_resource& after, bool flush_immediate = false);

	/**
	 * @brief 将资源屏障提交到命令列表
	 */
	void flush_resource_barriers();

	void insert_timestamp(ID3D12QueryHeap* query_heap, uint32_t query_index);
	void resolve_timestamps(ID3D12Resource* readback_heap, ID3D12QueryHeap* query_heap, uint32_t num_queries);
	void pix_begin_event(wchar_t const* label);
	void pix_end_event();
	void pix_set_marker(wchar_t const* label);

	void set_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, ID3D12DescriptorHeap* heap);
	void set_descriptor_heaps(UINT heap_count, D3D12_DESCRIPTOR_HEAP_TYPE heap_types[], ID3D12DescriptorHeap* heaps[]);
	void set_pipeline_state(pso const& pso);

	void set_predication(ID3D12Resource* Buffer, UINT64 BufferOffset, D3D12_PREDICATION_OP Op);

	/**
	 * @brief 
	 * @param id 
	 */
	void set_id(std::wstring const& id);

	/**
	 * @brief 命令列表类型
	 * @return 
	 */
	D3D12_COMMAND_LIST_TYPE get_type() const;

private:
	/**
	 * @brief 
	 */
	void bind_descriptor_heaps();

	command_context() = default;
	command_context(const command_context&) = delete;
	command_context& operator=(const command_context&) = delete;

protected:
	D3D12_COMMAND_LIST_TYPE type_; ///< 命令列表类型
	std::wstring id_; ///< 
	command_list_manager* owning_manager_; ///< 
	ID3D12GraphicsCommandList* rhi_command_list_; ///< 
	ID3D12CommandAllocator* rhi_current_allocator_; ///< 

	ID3D12RootSignature* rhi_cur_graphics_root_signature_; ///< 
	ID3D12RootSignature* rhi_cur_compute_root_signature_; ///< 
	ID3D12PipelineState* rhi_cur_pipeline_state_; ///< 

	dynamic_descriptor_heap dynamic_view_descriptor_heap_; ///< CBV_SRV_UAV描述符堆
	dynamic_descriptor_heap dynamic_sampler_descriptor_heap_; ///< SAMPLER描述符堆

	D3D12_RESOURCE_BARRIER resource_barrier_buffers_[16]; ///< 缓存的资源屏障，等待提交命令列表
	UINT num_barriers_to_flush_; ///< 缓存的个数

	ID3D12DescriptorHeap* current_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	linear_allocator cpu_linear_allocator_; ///< 
	linear_allocator gpu_linear_allocator_; ///< 
};

inline void command_context::set_id(std::wstring const& id)
{
	id_ = id;
}

inline D3D12_COMMAND_LIST_TYPE command_context::get_type() const
{
	return type_;
}

inline ID3D12GraphicsCommandList* command_context::get_command_list()
{
	return rhi_command_list_;
}

inline dyn_alloc command_context::reserve_upload_memory(size_t num_bytes)
{
	return cpu_linear_allocator_.allocate(num_bytes);
}
