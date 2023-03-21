#pragma once

#include "color_buffer.h"
#include "depth_buffer.h"
#include "gpu_buffer.h"

class buffer_manager
{
public:
	static void initialize_rendering_buffers(uint32_t width, uint32_t height);
	static void destroy_rendering_buffers();
	static void resize_display_dependent_buffers(uint32_t width, uint32_t height);
};
