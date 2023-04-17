#include "buffer_manager.h"

void buffer_manager::initialize_rendering_buffers(uint32_t width, uint32_t height)
{
	scene_depth_buffer.reset(new depth_buffer(0, 0));
	scene_depth_buffer->create(L"Scene Depth Buffer", width, height, DXGI_FORMAT_D32_FLOAT, 0);

	scene_color_buffer.reset(new color_buffer(color(0, 0, 1, 1)));
	scene_color_buffer->create(L"Main Color Buffer", width, height, 1, DXGI_FORMAT_R11G11B10_FLOAT, 0);
}

void buffer_manager::destroy_rendering_buffers()
{
}

void buffer_manager::resize_display_dependent_buffers(uint32_t width, uint32_t height)
{
}
