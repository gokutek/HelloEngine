#include "command_context_manager.h"
#include "utility.h"
#include "graphics_context.h"
#include "compute_context.h"

command_context_manager::command_context_manager()
{
}

command_context_manager::~command_context_manager()
{
}

command_context* command_context_manager::allocate_context(D3D12_COMMAND_LIST_TYPE type)
{
	ASSERT(type >= 0 && type < 4);
	command_context* context = nullptr;
	std::lock_guard<std::mutex> guard(mutex_);
	size_t index = (size_t)type;
	std::queue<command_context*>& queue = free_pools_[index];
	if (queue.empty())
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			context = new graphics_context(type);
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			context = new compute_context(type);
			break;
		}
		ASSERT(context);
		context_pools_[index].emplace_back(context);
		context->initialize();
	}
	else
	{
		context = queue.front();
		queue.pop();
		context->reset();
	}

	ASSERT(context);
	return context;
}

graphics_context* command_context_manager::allocate_graphics_context(wchar_t const* id)
{
	graphics_context* context = (graphics_context*)allocate_context(D3D12_COMMAND_LIST_TYPE_DIRECT);
	context->set_id(id);
	return context;
}

void command_context_manager::free_context(command_context* context)
{
	ASSERT(context);
	std::lock_guard<std::mutex> guard(mutex_);
	free_pools_[context->get_type()].push(context);
}

void command_context_manager::destroy_all_contexts()
{
	//TODO:
	assert(false);
}
