#pragma once

#include "command_list_manager.h"

namespace graphics
{
	extern command_list_manager command_manager;

	void initialize(bool RequireDXRSupport = false);
	void shutdown();
}
