#pragma once

#include "command_list_manager.h"

namespace graphics
{
	extern command_list_manager command_manager;
	extern ID3D12Device* device;

	void initialize(bool RequireDXRSupport = false);
	void shutdown();
}
