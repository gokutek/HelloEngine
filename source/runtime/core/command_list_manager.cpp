#include "command_list_manager.h"
#include "utility.h"

command_list_manager::command_list_manager() :
	device_(nullptr),
	graphics_queue_(D3D12_COMMAND_LIST_TYPE_DIRECT),
	compute_queue_(D3D12_COMMAND_LIST_TYPE_COMPUTE),
	copy_queue_(D3D12_COMMAND_LIST_TYPE_COPY)
{
}

command_list_manager::~command_list_manager()
{
	shutdown();
}

void command_list_manager::create(ID3D12Device* device)
{
	ASSERT(device);
	device_ = device;
	graphics_queue_.create(device);
	compute_queue_.create(device);
	copy_queue_.create(device);
}

void command_list_manager::shutdown()
{
	graphics_queue_.shutdown();
	compute_queue_.shutdown();
	copy_queue_.shutdown();
}

void command_list_manager::create_new_command_list(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator)
{
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		*allocator = graphics_queue_.request_allocator();
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		*allocator = compute_queue_.request_allocator();
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		*allocator = copy_queue_.request_allocator();
		break;
	default:
		ASSERT(false, "Not yet supported type");
		*allocator = nullptr;
		break;
	}

	ASSERT_SUCCEEDED(device_->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list)));
	(*list)->SetName(L"command_list_manager::create_new_command_list");
}

bool command_list_manager::is_fence_complete(uint64_t fence_value)
{
	command_queue& queue = get_queue(D3D12_COMMAND_LIST_TYPE(fence_value >> 56));
	return queue.is_fence_complete(fence_value);
}

void command_list_manager::wait_for_fence(uint64_t fence_value)
{
	//FIXME: 原本是访问的全局对象
	command_queue& producer = get_queue(D3D12_COMMAND_LIST_TYPE(fence_value >> 56));
	producer.wait_for_fence(fence_value);
}

void command_list_manager::idle_gpu()
{
	graphics_queue_.wait_for_idle();
	compute_queue_.wait_for_idle();
	copy_queue_.wait_for_idle();
}
