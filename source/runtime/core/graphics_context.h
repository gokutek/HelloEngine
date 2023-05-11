#pragma once

#include "command_context.h"
#include "gpu_buffer.h"
#include "color_buffer.h"
#include "depth_buffer.h"
#include "root_signature.h"
#include "command_signature.h"

/**
 * @brief 向Command List中插入各种渲染命令
 */
class graphics_context : public command_context
{
public:
	using command_context::command_context;

    void clear_uav(gpu_buffer& target);
    void clear_uav(color_buffer& target);
    void clear_color(color_buffer& target, D3D12_RECT const* rect);
    void clear_color(color_buffer& target, float color[4], D3D12_RECT const* rect);
    void clear_depth(depth_buffer& target);
    void clear_stencil(depth_buffer& target);
    void clear_depth_and_stencil(depth_buffer& target);

    void BeginQuery(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT heap_index);
    void EndQuery(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT heap_index);
    void ResolveQueryData(ID3D12QueryHeap* query_heap, D3D12_QUERY_TYPE query_type, UINT start_index, UINT num_queries, ID3D12Resource* destination_buffer, UINT64 destination_buffer_offset);

    void set_root_signature(root_signature const& root_sig);

    void set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array);
    void set_render_targets(UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array, D3D12_CPU_DESCRIPTOR_HANDLE dsv);
    void set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE rtv) { set_render_targets(1, &rtv); }
    void set_render_target(D3D12_CPU_DESCRIPTOR_HANDLE rtv, D3D12_CPU_DESCRIPTOR_HANDLE dsv) { set_render_targets(1, &rtv, dsv); }
    void set_depth_stencil_target(D3D12_CPU_DESCRIPTOR_HANDLE dsv) { set_render_targets(0, nullptr, dsv); }

    void set_viewport(D3D12_VIEWPORT const& viewport);
    void set_viewport(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);
    void set_scissor(D3D12_RECT const& rect);
    void set_scissor(UINT left, UINT top, UINT right, UINT bottom);
    void set_viewport_and_scissor(D3D12_VIEWPORT const& viewport, D3D12_RECT const& rect);
    void set_viewport_and_scissor(UINT x, UINT y, UINT w, UINT h);
    void set_stencil_ref(UINT stencil_ref);
    void set_blend_factor(color blend_factor);
    void set_primitive_topology(D3D12_PRIMITIVE_TOPOLOGY topology);

    void set_constant_array(UINT root_index, UINT num_constants, void const* constants);
    void set_constant(UINT root_index, UINT offset, UINT val);
    void set_constants(UINT root_index, UINT x);
    void set_constants(UINT root_index, UINT x, UINT y);
    void set_constants(UINT root_index, UINT x, UINT y, UINT z);
    void set_constants(UINT root_index, UINT x, UINT y, UINT z, UINT w);
    void set_constant_buffer(UINT root_index, D3D12_GPU_VIRTUAL_ADDRESS cbv);
    void set_dynamic_constant_buffer_view(UINT root_index, size_t buffer_size, void const* buffer_data);
    void set_buffer_srv(UINT root_index, gpu_buffer const& srv, UINT64 offset = 0);
    void set_buffer_uav(UINT root_index, gpu_buffer const& uav, UINT64 offset = 0);
    void set_descriptor_table(UINT root_index, D3D12_GPU_DESCRIPTOR_HANDLE first_handle);

    void set_dynamic_descriptor(UINT root_index, UINT offset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
    void set_dynamic_descriptors(UINT root_index, UINT offset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles);
    void set_dynamic_sampler(UINT root_index, UINT offset, D3D12_CPU_DESCRIPTOR_HANDLE handle);
    void set_dynamic_samplers(UINT root_index, UINT offset, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE const* handles);

    void set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view);
    void set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view);
    void set_vertex_buffers(UINT start_slot, UINT count,  D3D12_VERTEX_BUFFER_VIEW const* vb_views);
    void set_dynamic_vb(UINT slot, size_t num_vertices, size_t vertex_stride, void const* vb_data);
    void set_dynamic_ib(size_t index_count, uint16_t const* ib_data);
    void set_dynamic_srv(UINT root_index, size_t buffer_size, void const* buffer_data);

    void draw(uint32_t vertex_count, uint32_t vertex_start_offset);
    void draw_indexed(uint32_t index_count, uint32_t start_index_location, int base_vertex_location);
    void draw_instanced(uint32_t vertex_count_per_instance, uint32_t instance_count, uint32_t start_vertex_location, uint32_t start_instance_location);
    void draw_indexed_instanced(uint32_t index_count_per_instance, uint32_t instance_count, uint32_t start_index_location, uint32_t base_vertex_location, uint32_t start_instance_location);
    void draw_indirect(gpu_buffer& argument_buffer, uint64_t argument_buffer_offset = 0);
    void execute_indirect(command_signature& command_sig, gpu_buffer& argument_buffer, uint64_t argument_start_offset = 0, uint32_t max_commands = 1, gpu_buffer* command_counter_buffer = nullptr, uint64_t counter_offset = 0);
};
