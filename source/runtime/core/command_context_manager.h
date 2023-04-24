#pragma once

#include "graphics_context.h"
#include "compute_context.h"
#include <mutex>
#include <vector>
#include <queue>
#include <memory>

class command_context_manager
{
public:
	command_context_manager();
	~command_context_manager();

	graphics_context* allocate_graphics_context(wchar_t const* id);
	void free_context(command_context* context);
	void destroy_all_contexts();

private:
	command_context* allocate_context(D3D12_COMMAND_LIST_TYPE type);

private:
	std::vector<std::unique_ptr<command_context> > context_pools_[4];
	std::queue<command_context*> free_pools_[4];
	std::mutex mutex_;
};

/*
===============================================================================
1. 对应原来的ContextManager；
===============================================================================
*/
