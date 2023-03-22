#pragma once

#include "color_buffer.h"
#include "depth_buffer.h"
#include "gpu_buffer.h"

class buffer_manager
{
public:
	void initialize_rendering_buffers(uint32_t width, uint32_t height);
	void destroy_rendering_buffers();
	void resize_display_dependent_buffers(uint32_t width, uint32_t height);

	depth_buffer* scene_depth_buffer;
	color_buffer* scene_color_buffer;
};
