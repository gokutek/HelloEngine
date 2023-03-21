#include "command_context_manager.h"
#include "utility.h"

command_context_manager::command_context_manager()
{
}

command_context* command_context_manager::allocate_context(D3D12_COMMAND_LIST_TYPE type)
{
	ASSERT(type >= 0 && type < 4);
	command_context* ret = nullptr;
	std::lock_guard<std::mutex> guard(mutex_);
	size_t index = (size_t)type;
	std::queue<command_context*>& queue = free_pools_[index];
	if (queue.empty())
	{
		ret = new command_context(type);
		context_pools_[index].emplace_back(ret);
		ret->initialize();
	}
	else
	{
		ret = queue.front();
		queue.pop();
		ret->reset();
	}

	ASSERT(ret);
	return ret;
}

void command_context_manager::free_context(command_context* context)
{
	//TODO:
}

void command_context_manager::destroy_all_contexts()
{
	//TODO:
}
