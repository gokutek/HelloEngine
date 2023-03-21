#pragma once

#include "command_context.h"
#include "gpu_buffer.h"
#include "color_buffer.h"
#include "depth_buffer.h"

class graphics_context : public command_context
{
public:
	static graphics_context& begin(wchar_t const* id);

	void clear_uav(gpu_buffer& target);
	void clear_uav(color_buffer& target);
	void clear_color(color_buffer& target, D3D12_RECT const* rect);
	void clear_depth(depth_buffer& target);
	void clear_stencil(depth_buffer& target);
	void clear_depth_and_stencil(depth_buffer& target);

	void set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view);
	void set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view);

	void draw(uint32_t vertex_count, uint32_t vertex_start_offset);
	void draw_indexed(uint32_t index_count, uint32_t start_index_location, int base_vertex_location);
};
