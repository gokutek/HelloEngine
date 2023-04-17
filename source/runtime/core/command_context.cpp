#include "command_context.h"
#include "command_context_manager.h"
#include "graphics_core.h"
#include "utility.h"

command_context& command_context::begin(wchar_t const* id)
{
	command_context* new_context = get_rhi()->context_manager_.allocate_context(D3D12_COMMAND_LIST_TYPE_DIRECT);
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
    ASSERT(type_ == D3D12_COMMAND_LIST_TYPE_DIRECT || type_ == D3D12_COMMAND_LIST_TYPE_COMPUTE);

    flush_resource_barriers();

    ASSERT(rhi_current_allocator_ != nullptr);

    command_queue& cmd_queue = get_rhi()->command_manager_.get_queue(type_);

    uint64_t FenceValue = cmd_queue.execute_command_list(rhi_command_list_);
    cmd_queue.discard_allocator(FenceValue, rhi_current_allocator_);
    rhi_current_allocator_ = nullptr;

    //m_CpuLinearAllocator.CleanupUsedPages(FenceValue);
    //m_GpuLinearAllocator.CleanupUsedPages(FenceValue);
    //m_DynamicViewDescriptorHeap.CleanupUsedHeaps(FenceValue);
    //m_DynamicSamplerDescriptorHeap.CleanupUsedHeaps(FenceValue);

    if (wait_for_completion)
    {
        get_rhi()->command_manager_.wait_for_fence(FenceValue);
    }

    get_rhi()->context_manager_.free_context(this);

    return FenceValue;
}

void command_context::initialize()
{
	get_rhi()->command_manager_.create_new_command_list(type_, &rhi_command_list_, &rhi_current_allocator_);
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
