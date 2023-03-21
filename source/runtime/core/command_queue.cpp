#include "command_queue.h"
#include "utility.h"
#include "graphics_core.h"

command_queue::command_queue(D3D12_COMMAND_LIST_TYPE Type) :
	type_(Type),
	rhi_command_queue_(nullptr),
	fence_(nullptr),
	next_fence_value_((uint64_t)Type << 56 | 1),
	last_completed_fence_value_((uint64_t)Type << 56),
	allocator_pool_(Type)
{
}

command_queue::~command_queue()
{
	shutdown();
}

void command_queue::create(ID3D12Device* device)
{
	//ȷ�������ظ�����create...
	ASSERT(device != nullptr);
	ASSERT(!is_ready());
	ASSERT(allocator_pool_.size() == 0);

	//����queue
	D3D12_COMMAND_QUEUE_DESC queue_desc;
	queue_desc.Type = type_;
	queue_desc.Priority = 0;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.NodeMask = 1; //???
	device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&rhi_command_queue_));
	rhi_command_queue_->SetName(L"command_queue::rhi_command_queue_");

	//����fence
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fence_->SetName(L"command_queue::fence_");
	fence_->Signal((uint64_t)type_ << 56);

	fence_event_handle_ = CreateEvent(nullptr, false, false, nullptr);
	ASSERT(fence_event_handle_ != nullptr);

	allocator_pool_.create(device);

	ASSERT(is_ready());
}

void command_queue::shutdown()
{
	allocator_pool_.shutdown();

	CloseHandle(fence_event_handle_);

	fence_->Release();
	fence_ = nullptr;

	rhi_command_queue_->Release();
	rhi_command_queue_ = nullptr;
}

uint64_t command_queue::increment_fence()
{
	std::lock_guard<std::mutex> guard(fence_mutex_);
	rhi_command_queue_->Signal(fence_, next_fence_value_);
	return next_fence_value_++;
}

bool command_queue::is_fence_complete(uint64_t fence_value)
{
	// Avoid querying the fence value by testing against the last one seen.
	// The max() is to protect against an unlikely race condition that could cause the last
	// completed fence value to regress.
	if (fence_value > last_completed_fence_value_)
	{
		last_completed_fence_value_ = std::max(last_completed_fence_value_, fence_->GetCompletedValue());
	}

	return fence_value <= last_completed_fence_value_;
}

void command_queue::stall_for_fence(uint64_t fence_value)
{
	D3D12_COMMAND_LIST_TYPE type = (D3D12_COMMAND_LIST_TYPE)(fence_value >> 56);
	command_queue& producer = graphics::command_manager.get_queue(type);
	rhi_command_queue_->Wait(producer.get_fence(), fence_value);
}

void command_queue::stall_for_producer(command_queue& producer)
{
	ASSERT(producer.next_fence_value_ > 0);
	rhi_command_queue_->Wait(producer.get_fence(), producer.next_fence_value_ - 1);
}

void command_queue::wait_for_fence(uint64_t fence_value)
{
	if (is_fence_complete(fence_value))
	{
		return;
	}

	// TODO:  Think about how this might affect a multi-threaded situation.  Suppose thread A
	// wants to wait for fence 100, then thread B comes along and wants to wait for 99.  If
	// the fence can only have one event set on completion, then thread B has to wait for 
	// 100 before it knows 99 is ready.  Maybe insert sequential events?
	{
		std::lock_guard<std::mutex> guard(event_mutex_);
		fence_->SetEventOnCompletion(fence_value, fence_event_handle_);
		WaitForSingleObject(fence_event_handle_, INFINITE);
		last_completed_fence_value_ = fence_value;
	}
}

uint64_t command_queue::execute_command_list(ID3D12CommandList* list)
{
	std::lock_guard<std::mutex> guard(fence_mutex_);

	ASSERT_SUCCEEDED(((ID3D12GraphicsCommandList*)list)->Close());

	//�ύlist
	rhi_command_queue_->ExecuteCommandLists(1, &list);

	//��GPU������fence��ֵ��������в���һ��sigal���
	rhi_command_queue_->Signal(fence_, next_fence_value_);

	return next_fence_value_++;
}

ID3D12CommandAllocator* command_queue::request_allocator()
{
	uint64_t completed_fence = fence_->GetCompletedValue();
	return allocator_pool_.request_allocator(completed_fence);
}

void command_queue::discard_allocator(uint64_t fence_value_for_reset, ID3D12CommandAllocator* allocator)
{
	allocator_pool_.discard_allocator(fence_value_for_reset, allocator);
}