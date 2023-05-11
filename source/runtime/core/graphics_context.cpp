#include "graphics_context.h"

void graphics_context::clear_uav(gpu_buffer& target)
{
	//TODO:
}

void graphics_context::clear_uav(color_buffer& target)
{
	//TODO:
}

void graphics_context::clear_color(color_buffer& target, D3D12_RECT const* rect)
{
	flush_resource_barriers();
	color clear_color = target.get_clear_color();
	rhi_command_list_->ClearRenderTargetView(target.get_rtv(), clear_color.get_ptr(), rect ? 1 : 0, rect);
}

void graphics_context::clear_color(color_buffer& target, float color[4], D3D12_RECT const* rect)
{
}

void graphics_context::clear_depth(depth_buffer& target)
{
	flush_resource_barriers();
	//TODO:
}

void graphics_context::clear_stencil(depth_buffer& target)
{
	//TODO:
}

void graphics_context::clear_depth_and_stencil(depth_buffer& target)
{
	//TODO:
}

void graphics_context::BeginQuery(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT heap_index)
{
}

void graphics_context::EndQuery(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT heap_index)
{
}

void graphics_context::ResolveQueryData(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT start_index, UINT num_queries, ID3D12Resource* destination_buffer, UINT64 destination_buffer_offset)
{
}

void graphics_context::set_root_signature(root_signature const& root_sig)
{
}

void graphics_context::set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array)
{
	rhi_command_list_->OMSetRenderTargets(count, rtv_array, FALSE, nullptr);
}

void graphics_context::set_render_targets(UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array, D3D12_CPU_DESCRIPTOR_HANDLE dsv)
{
}

void graphics_context::set_viewport(D3D12_VIEWPORT const& viewport)
{
}

void graphics_context::set_viewport(float x, float y, float w, float h, float min_depth, float max_depth)
{
}

void graphics_context::set_scissor(D3D12_RECT const& rect)
{
}

void graphics_context::set_scissor(UINT left, UINT top, UINT right, UINT bottom)
{
}

void graphics_context::set_viewport_and_scissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect)
{
	rhi_command_list_->RSSetViewports(1, &viewport);
	rhi_command_list_->RSSetScissorRects(1, &rect);
}

void graphics_context::set_viewport_and_scissor(UINT x, UINT y, UINT w, UINT h)
{
}

void graphics_context::set_stencil_ref(UINT stencil_ref)
{
}

void graphics_context::set_blend_factor(color blend_factor)
{
}

void graphics_context::set_primitive_topology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	rhi_command_list_->IASetPrimitiveTopology(topology);
}

void graphics_context::set_constant_array(UINT root_index, UINT num_constants, void const* constants)
{
}

void graphics_context::set_constant(UINT root_index, UINT offset, UINT val)
{
}

void graphics_context::set_constants(UINT root_index, UINT x)
{
}

void graphics_context::set_constants(UINT root_index, UINT x, UINT y)
{
}

void graphics_context::set_constants(UINT root_index, UINT x, UINT y, UINT z)
{
}

void graphics_context::set_constants(UINT root_index, UINT x, UINT y, UINT z, UINT w)
{
}

void graphics_context::set_constant_buffer(UINT root_index, D3D12_GPU_VIRTUAL_ADDRESS cbv)
{
}

void graphics_context::set_dynamic_constant_buffer_view(UINT root_index, size_t buffer_size, void const* buffer_data)
{
}

void graphics_context::set_buffer_srv(UINT root_index, gpu_buffer const& srv, UINT64 offset)
{
}

void graphics_context::set_buffer_uav(UINT root_index, gpu_buffer const& uav, UINT64 offset)
{
}

void graphics_context::set_descriptor_table(UINT root_index, D3D12_GPU_DESCRIPTOR_HANDLE first_handle)
{
}

void graphics_context::set_dynamic_descriptor(UINT root_index, UINT offset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
}

void graphics_context::set_dynamic_descriptors(UINT root_index, UINT offset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
}

void graphics_context::set_dynamic_sampler(UINT root_index, UINT offset, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
}

void graphics_context::set_dynamic_samplers(UINT root_index, UINT offset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
}

void graphics_context::set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view)
{
	rhi_command_list_->IASetIndexBuffer(&view);
}

void graphics_context::set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view)
{
	rhi_command_list_->IASetVertexBuffers(slot, 1, &view);
}

void graphics_context::set_vertex_buffers(UINT start_slot, UINT count, D3D12_VERTEX_BUFFER_VIEW const* vb_views)
{
}

void graphics_context::set_dynamic_vb(UINT slot, size_t num_vertices, size_t vertex_stride, void const* vb_data)
{
}

void graphics_context::set_dynamic_ib(size_t index_count, uint16_t const* ib_data)
{
}

void graphics_context::set_dynamic_srv(UINT root_index, size_t buffer_size, void const* buffer_data)
{
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
	//TODO:
}

void graphics_context::draw_indexed_instanced(uint32_t index_count_per_instance, uint32_t instance_count, uint32_t start_index_location, uint32_t base_vertex_location, uint32_t start_instance_location)
{
	//TODO:
}

void graphics_context::draw_indirect(gpu_buffer& argument_buffer, uint64_t argument_buffer_offset)
{
}

void graphics_context::execute_indirect(command_signature& command_sig, gpu_buffer& argument_buffer, uint64_t argument_start_offset, uint32_t max_commands, gpu_buffer* command_counter_buffer, uint64_t counter_offset)
{
}
