#include "graphics_context.h"

graphics_context& graphics_context::begin(wchar_t const* id)
{
	//FIXME: 这里原初的设计不是很好
	graphics_context* context = (graphics_context*)&command_context::begin(id);
	return *context;
}

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

void graphics_context::set_render_targets(uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE const* rtv_array)
{
	rhi_command_list_->OMSetRenderTargets(count, rtv_array, FALSE, nullptr);
}

void graphics_context::set_viewport_and_scissor(const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect)
{
	rhi_command_list_->RSSetViewports(1, &viewport);
	rhi_command_list_->RSSetScissorRects(1, &rect);
}

void graphics_context::set_index_buffer(D3D12_INDEX_BUFFER_VIEW const& view)
{
	//TODO:
}

void graphics_context::set_vertex_buffer(uint32_t slot, D3D12_VERTEX_BUFFER_VIEW const& view)
{
	//TODO:
}

void graphics_context::draw(uint32_t vertex_count, uint32_t vertex_start_offset)
{
	//TODO:
}

void graphics_context::draw_indexed(uint32_t index_count, uint32_t start_index_location, int base_vertex_location)
{
	//TODO:
}
