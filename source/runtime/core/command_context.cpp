#include "command_context.h"
#include "command_context_manager.h"
#include "graphics_core.h"

command_context& command_context::begin(wchar_t const* id)
{
	command_context* new_context = graphics::context_manager.allocate_context(D3D12_COMMAND_LIST_TYPE_DIRECT);
	new_context->set_id(id);
	//TODO: profiling
	return *new_context;
}

command_context::command_context(D3D12_COMMAND_LIST_TYPE type) :
	type_(type)
{
}

command_context::~command_context()
{
}

uint64_t command_context::flush(bool wait_for_completion)
{
	//TODO:
	return 0;
}

uint64_t command_context::finish(bool wait_for_completion)
{
	//TODO:
	return 0;
}

void command_context::initialize()
{
	//TODO:
}

void command_context::reset()
{
	//TODO:
}

ID3D12GraphicsCommandList* command_context::get_command_list()
{
	//TODO:
	return nullptr;
}
