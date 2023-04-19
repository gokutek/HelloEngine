#include "command_context_manager.h"
#include "utility.h"

command_context_manager::command_context_manager()
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
		context = new command_context(type);
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
