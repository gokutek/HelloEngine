#pragma once

#include "command_context.h"
#include "gpu_buffer.h"
#include "color_buffer.h"
#include "depth_buffer.h"

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
	void clear_depth(depth_buffer& target);
	void clear_stencil(depth_buffer& target);
	void clear_depth_and_stencil(depth_buffer& target);

	void set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array);

	void set_viewport_and_scissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect);
	void set_primitive_topology(D3D12_PRIMITIVE_TOPOLOGY topology);

	void set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view);
	void set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view);

	void draw(uint32_t vertex_count, uint32_t vertex_start_offset);
	void draw_indexed(uint32_t index_count, uint32_t start_index_location, int base_vertex_location);
	void draw_instanced(uint32_t vertex_count_per_instance, uint32_t instance_count, uint32_t start_vertex_location, uint32_t start_instance_location);
};
