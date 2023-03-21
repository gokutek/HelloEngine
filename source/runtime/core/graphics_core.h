#pragma once

#include "command_list_manager.h"
#include "command_context_manager.h"

namespace graphics
{
	extern ID3D12Device* device;
	extern command_list_manager command_manager;
	extern command_context_manager context_manager;

	void initialize(bool RequireDXRSupport = false);
	void shutdown();
}
