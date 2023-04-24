#include "command_context.h"
#include "command_context_manager.h"
#include "graphics_core.h"
#include "utility.h"

//command_context& command_context::begin(wchar_t const* id)
//{
//	command_context* new_context = get_rhi()->context_manager_.allocate_context(D3D12_COMMAND_LIST_TYPE_DIRECT);
//	new_context->set_id(id);
//	//TODO: profiling
//	return *new_context;
//}

command_context::command_context(D3D12_COMMAND_LIST_TYPE type) 
	: type_(type),
	owning_manager_(nullptr),
	rhi_command_list_(nullptr),
	rhi_current_allocator_(nullptr),
	rhi_cur_graphics_root_signature_(nullptr),
	rhi_cur_compute_root_signature_(nullptr),
	rhi_cur_pipeline_state_(nullptr),
	num_barriers_to_flush_(0)
{
	ZeroMemory(resource_barrier_buffers_, sizeof(resource_barrier_buffers_));
}

command_context::~command_context()
{
}

uint64_t command_context::flush(bool wait_for_completion)
{
	//TODO:
	assert(false);
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
	// We only call Reset() on previously freed contexts.  The command list persists, but we must
	// request a new allocator.
	ASSERT(rhi_command_list_ != nullptr && rhi_current_allocator_ == nullptr);
	rhi_current_allocator_ = get_rhi()->command_manager_.get_queue(type_).request_allocator();
	rhi_command_list_->Reset(rhi_current_allocator_, nullptr);

	rhi_cur_graphics_root_signature_ = nullptr;
	rhi_cur_compute_root_signature_ = nullptr;
	rhi_cur_pipeline_state_ = nullptr;
	num_barriers_to_flush_ = 0;

	bind_descriptor_heaps();
}

ID3D12GraphicsCommandList* command_context::get_command_list()
{
	//TODO:
	assert(false);
	return nullptr;
}

void command_context::transition_resource(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate)
{
	D3D12_RESOURCE_STATES old_state = resource.get_usage_state();
	if (type_ == D3D12_COMMAND_LIST_TYPE_COMPUTE)
	{
		//TODO:
	}

	if (new_state != old_state)
	{
		ASSERT(num_barriers_to_flush_ < _countof(resource_barrier_buffers_));
		D3D12_RESOURCE_BARRIER& barrier_desc = resource_barrier_buffers_[num_barriers_to_flush_++];
		barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_desc.Transition.pResource = resource.get_resource();
		barrier_desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier_desc.Transition.StateBefore = old_state;
		barrier_desc.Transition.StateAfter = new_state;

		if (new_state == resource.get_transitioning_state())
		{
			barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
			resource.set_transitioning_state(D3D12_RESOURCE_STATES(-1));
		}
		else
		{
			barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		}

		resource.set_usage_state(new_state);
	}
	else if (new_state == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		//TODO:
		assert(false);
	}

	if (flush_immediate || num_barriers_to_flush_ == _countof(resource_barrier_buffers_))
	{
		flush_resource_barriers();
	}
}

void command_context::flush_resource_barriers()
{
	if (num_barriers_to_flush_ > 0)
	{
		rhi_command_list_->ResourceBarrier(num_barriers_to_flush_, resource_barrier_buffers_);
		num_barriers_to_flush_ = 0;
	}
}

void command_context::bind_descriptor_heaps()
{
#if 0
	UINT NonNullHeaps = 0;
	ID3D12DescriptorHeap* HeapsToBind[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	for (UINT i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		ID3D12DescriptorHeap* HeapIter = m_CurrentDescriptorHeaps[i];
		if (HeapIter != nullptr)
			HeapsToBind[NonNullHeaps++] = HeapIter;
	}

	if (NonNullHeaps > 0)
		m_CommandList->SetDescriptorHeaps(NonNullHeaps, HeapsToBind);
#endif
}
