#pragma once

#include "command_context.h"
#include "gpu_buffer.h"

class graphics_context : public command_context
{
public:
	static graphics_context& begin(const wchar_t* ID);

	void clear_uav(gpu_buffer& target);
};
