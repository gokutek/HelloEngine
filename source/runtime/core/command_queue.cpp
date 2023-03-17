#include "command_queue.h"

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
	//TODO:

	//创建queue
	D3D12_COMMAND_QUEUE_DESC queue_desc;
	queue_desc.Type = type_;
	queue_desc.Priority = 0;
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.NodeMask = 1; //???
	device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&rhi_command_queue_));
	rhi_command_queue_->SetName(L"");

	//创建fence
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	fence_->SetName(L"");

	//TODO：
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
