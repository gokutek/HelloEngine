#pragma once

#include "pch.h"
#include "gpu_resource.h"
#include "gpu_buffer.h"
#include "dynamic_descriptor_heap.h"
#include "linear_allocator.h"
#include <stdint.h>
#include <string>

class command_list_manager;

/**
 * @brief 在每一帧记录渲染命令时，都需要一个command_context。
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
	 * @brief 
	 * @param wait_for_completion 
	 * @return 
	 */
	uint64_t flush(bool wait_for_completion);

	/**
	 * @brief 
	 * @param wait_for_completion 
	 * @return 
	 */
	uint64_t finish(bool wait_for_completion);
	
	/**
	 * @brief 
	 */
	void initialize();

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
	 * @brief 
	 * @param dst 
	 * @param src 
	 */
	void copy_buffer(gpu_resource& dst, gpu_resource& src);

	/**
	 * @brief 
	 * @param dest 
	 * @param dest_offset 
	 * @param src 
	 * @param src_offset 
	 * @param NumBytes 
	 */
	void copy_buffer_region(gpu_resource& dest, size_t dest_offset, gpu_resource& src, size_t src_offset, size_t NumBytes);

	/**
	 * @brief 
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
	 * @param Source 
	 * @param rect 
	 */
	void copy_texture_region(gpu_resource& dest, UINT x, UINT y, UINT z, gpu_resource& Source, RECT& rect);

	//void reset_counter(StructuredBuffer& buf, uint32_t value = 0);

	//TODO:

	/**
	 * @brief 
	 * @param dest 
	 * @param num_sub_resources 
	 * @param subdata 
	 */
	void initialize_texture(gpu_resource& dest, UINT num_sub_resources, D3D12_SUBRESOURCE_DATA subdata[]);

	/**
	 * @brief 
	 * @param dest 
	 * @param data 
	 * @param num_bytes 
	 * @param dest_offset 
	 */
	void initialize_buffer(gpu_buffer& dest, const void* data, size_t num_bytes, size_t dest_offset = 0);

	/**
	 * @brief 
	 * @param dest 
	 * @param src 
	 * @param src_offset 
	 * @param num_bytes 
	 * @param dest_offset 
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
	 * @brief 
	 * @param resource 
	 * @param new_state 
	 * @param flush_immediate 
	 */
	void transition_resource(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate);

	/**
	 * @brief 
	 */
	void flush_resource_barriers();

	/**
	 * @brief 
	 * @param id 
	 */
	void set_id(std::wstring const& id);

	/**
	 * @brief 
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
	D3D12_COMMAND_LIST_TYPE type_;
	std::wstring id_;
	command_list_manager* owning_manager_;
	ID3D12GraphicsCommandList* rhi_command_list_;
	ID3D12CommandAllocator* rhi_current_allocator_;

	ID3D12RootSignature* rhi_cur_graphics_root_signature_;
	ID3D12RootSignature* rhi_cur_compute_root_signature_;
	ID3D12PipelineState* rhi_cur_pipeline_state_;

	dynamic_descriptor_heap dynamic_view_descriptor_heap_;		// HEAP_TYPE_CBV_SRV_UAV
	dynamic_descriptor_heap dynamic_sampler_descriptor_heap_;	// HEAP_TYPE_SAMPLER

	D3D12_RESOURCE_BARRIER resource_barrier_buffers_[16];
	UINT num_barriers_to_flush_;

	ID3D12DescriptorHeap* current_descriptor_heaps_[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

	linear_allocator cpu_linear_allocator_;
	linear_allocator gpu_linear_allocator_;
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
