#pragma once

#include "pch.h"
#include "gpu_resource.h"
#include "gpu_buffer.h"
#include <stdint.h>
#include <string>

class command_list_manager;

/*
===============================================================================
子类有graphics_context和compute_context。
===============================================================================
*/
class command_context
{
public:
	command_context(D3D12_COMMAND_LIST_TYPE type);
	virtual ~command_context();

	uint64_t flush(bool wait_for_completion);
	uint64_t finish(bool wait_for_completion);
	
	void initialize();
	void reset();

	ID3D12GraphicsCommandList* get_command_list();
	void copy_buffer(gpu_resource& dst, gpu_resource& src);
	void copy_buffer_region(gpu_resource& dest, size_t dest_offset, gpu_resource& src, size_t src_offset, size_t NumBytes);
	void copy_sub_resource(gpu_resource& dest, UINT dest_sub_index, gpu_resource& src, UINT src_sub_index);
	//void copy_counter(gpu_resource& dest, size_t dest_offset, StructuredBuffer& src);
	void copy_texture_region(gpu_resource& dest, UINT x, UINT y, UINT z, gpu_resource& Source, RECT& rect);
	//void reset_counter(StructuredBuffer& buf, uint32_t value = 0);

	//TODO:
	void initialize_texture(gpu_resource& dest, UINT num_sub_resources, D3D12_SUBRESOURCE_DATA subdata[]);
	void initialize_buffer(gpu_buffer& dest, const void* data, size_t num_bytes, size_t dest_offset = 0);
	void initialize_buffer(gpu_buffer& dest, const upload_buffer& src, size_t src_offset, size_t num_bytes = -1, size_t dest_offset = 0);
	void initialize_texture_array_slice(gpu_resource& dest, UINT slice_index, gpu_resource& src);

	void write_buffer(gpu_resource& dest, size_t dest_offset, const void* data, size_t num_bytes);
	//void fill_buffer(gpu_resource& dest, size_t dest_offset, DWParam Value, size_t num_bytes);

	void transition_resource(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate);

	void flush_resource_barriers();

	void set_id(std::wstring const& id);

	D3D12_COMMAND_LIST_TYPE get_type() const;

private:
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

	D3D12_RESOURCE_BARRIER resource_barrier_buffers_[16];
	UINT num_barriers_to_flush_;
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
