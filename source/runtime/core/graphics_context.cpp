#include "graphics_context.h"

void graphics_context::clear_uav(gpu_buffer& target)
{
	assert(false && "unimpl yet!");
}

void graphics_context::clear_uav(color_buffer& target)
{
	assert(false && "unimpl yet!");
}

void graphics_context::clear_color(color_buffer& target, D3D12_RECT const* rect)
{
	flush_resource_barriers();
	color clear_color = target.get_clear_color();
	rhi_command_list_->ClearRenderTargetView(target.get_rtv(), clear_color.get_ptr(), rect ? 1 : 0, rect);
}

void graphics_context::clear_color(color_buffer& target, float color[4], D3D12_RECT const* rect)
{
	assert(false && "unimpl yet!");
}

void graphics_context::clear_depth(depth_buffer& target)
{
	flush_resource_barriers();
	assert(false && "unimpl yet!");
}

void graphics_context::clear_stencil(depth_buffer& target)
{
	assert(false && "unimpl yet!");
}

void graphics_context::clear_depth_and_stencil(depth_buffer& target)
{
	assert(false && "unimpl yet!");
}

void graphics_context::begin_query(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, uint32_t heap_index)
{
	assert(false && "unimpl yet!");
}

void graphics_context::end_query(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, uint32_t heap_index)
{
	assert(false && "unimpl yet!");
}

void graphics_context::resolve_query_data(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, uint32_t start_index, uint32_t num_queries, ID3D12Resource* destination_buffer, UINT64 destination_buffer_offset)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_root_signature(root_signature const& root_sig)
{
	if (root_sig.get_signature() == rhi_cur_graphics_root_signature_)
	{
		return;
	}

	rhi_cur_graphics_root_signature_ = root_sig.get_signature();
	rhi_command_list_->SetGraphicsRootSignature(rhi_cur_graphics_root_signature_);

	//TODO:
	//dynamic_view_descriptor_heap_.ParseGraphicsRootSignature(RootSig);
	//dynamic_sampler_descriptor_heap_.ParseGraphicsRootSignature(RootSig);
}

void graphics_context::set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array)
{
	rhi_command_list_->OMSetRenderTargets(count, rtv_array, FALSE, nullptr);
}

void graphics_context::set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_viewport(D3D12_VIEWPORT const& viewport)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_viewport(float x, float y, float w, float h, float min_depth, float max_depth)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_scissor(D3D12_RECT const& rect)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_scissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_viewport_and_scissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect)
{
	rhi_command_list_->RSSetViewports(1, &viewport);
	rhi_command_list_->RSSetScissorRects(1, &rect);
}

void graphics_context::set_viewport_and_scissor(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_stencil_ref(uint32_t stencil_ref)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_blend_factor(color blend_factor)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_primitive_topology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	rhi_command_list_->IASetPrimitiveTopology(topology);
}

void graphics_context::set_constant_array(uint32_t root_index, uint32_t num_constants, void const* constants)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constant(uint32_t root_index, uint32_t offset, uint32_t val)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constants(uint32_t root_index, uint32_t x)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constants(uint32_t root_index, uint32_t x, uint32_t y)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constants(uint32_t root_index, uint32_t x, uint32_t y, uint32_t z)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constants(uint32_t root_index, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_constant_buffer(uint32_t root_index, D3D12_GPU_VIRTUAL_ADDRESS cbv)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_constant_buffer_view(uint32_t root_index, size_t buffer_size, void const* buffer_data)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_buffer_srv(uint32_t root_index, gpu_buffer const& srv, UINT64 offset)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_buffer_uav(uint32_t root_index, gpu_buffer const& uav, UINT64 offset)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_descriptor_table(uint32_t root_index, D3D12_GPU_DESCRIPTOR_HANDLE first_handle)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_descriptor(uint32_t root_index, uint32_t offset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_descriptors(uint32_t root_index, uint32_t offset, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_sampler(uint32_t root_index, uint32_t offset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_samplers(uint32_t root_index, uint32_t offset, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view)
{
	rhi_command_list_->IASetIndexBuffer(&view);
}

void graphics_context::set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view)
{
	rhi_command_list_->IASetVertexBuffers(slot, 1, &view);
}

void graphics_context::set_vertex_buffers(uint32_t start_slot, uint32_t count, D3D12_VERTEX_BUFFER_VIEW const* vb_views)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_vb(uint32_t slot, size_t num_vertices, size_t vertex_stride, void const* vb_data)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_ib(size_t index_count, uint16_t const* ib_data)
{
	assert(false && "unimpl yet!");
}

void graphics_context::set_dynamic_srv(uint32_t root_index, size_t buffer_size, void const* buffer_data)
{
	assert(false && "unimpl yet!");
}

void graphics_context::draw(uint32_t vertex_count, uint32_t vertex_start_offset)
{
	draw_instanced(vertex_count, 1, vertex_start_offset, 0);
}

void graphics_context::draw_indexed(uint32_t index_count, uint32_t start_index_location, int base_vertex_location)
{
	draw_indexed_instanced(index_count, 1, start_index_location, base_vertex_location, 0);
}

void graphics_context::draw_instanced(uint32_t vertex_count_per_instance, uint32_t instance_count, uint32_t start_vertex_location, uint32_t start_instance_location)
{
	flush_resource_barriers();
	dynamic_view_descriptor_heap_.commit_graphics_root_descriptor_tables(rhi_command_list_);
	dynamic_sampler_descriptor_heap_.commit_graphics_root_descriptor_tables(rhi_command_list_);
	rhi_command_list_->DrawInstanced(vertex_count_per_instance, instance_count, start_vertex_location, start_instance_location);
}

void graphics_context::draw_indexed_instanced(uint32_t index_count_per_instance, uint32_t instance_count, uint32_t start_index_location, uint32_t base_vertex_location, uint32_t start_instance_location)
{
	assert(false && "unimpl yet!");
}

void graphics_context::draw_indirect(gpu_buffer& argument_buffer, uint64_t argument_buffer_offset)
{
	assert(false && "unimpl yet!");
}

void graphics_context::execute_indirect(command_signature& command_sig, gpu_buffer& argument_buffer, uint64_t argument_start_offset, uint32_t max_commands, gpu_buffer* command_counter_buffer, uint64_t counter_offset)
{
	assert(false && "unimpl yet!");
}
