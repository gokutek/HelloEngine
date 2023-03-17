#include "command_allocator_pool.h"
#include "utility.h"

command_allocator_pool::command_allocator_pool(D3D12_COMMAND_LIST_TYPE Type) :
	type_(Type),
	device_(nullptr)
{
}

command_allocator_pool::~command_allocator_pool()
{
	shutdown();
}

void command_allocator_pool::create(ID3D12Device* device)
{
	device_ = device;
}

void command_allocator_pool::shutdown()
{
	for (ID3D12CommandAllocator* allocator : allocator_pool_)
	{
		allocator->Release();
	}
	allocator_pool_.clear();
}

ID3D12CommandAllocator* command_allocator_pool::request_allocator(uint64_t CompletedFenceValue)
{
	std::lock_guard<std::mutex> guard(allocator_mutex_);

	ID3D12CommandAllocator* allocator = nullptr;

	//先尝试从池子里取
	if (!ready_allocators_.empty())
	{
		std::pair<uint64_t, ID3D12CommandAllocator*>& pair = ready_allocators_.front();
		if (pair.first <= CompletedFenceValue)
		{
			allocator = pair.second;
			ASSERT_SUCCEEDED(allocator->Reset());
			ready_allocators_.pop();
		}
	}
	
	//再尝试创建
	if (!allocator)
	{
		ASSERT_SUCCEEDED(device_->CreateCommandAllocator(type_, IID_PPV_ARGS(&allocator)));
		wchar_t name[32];
		swprintf(name, 32, L"CommandAllocator %zu", allocator_pool_.size());
		allocator->SetName(name);
		allocator_pool_.push_back(allocator);
	}

	return allocator;
}

void command_allocator_pool::discard_allocator(uint64_t FenceValue, ID3D12CommandAllocator* Allocator)
{
	std::lock_guard<std::mutex> guard(allocator_mutex_);
	ready_allocators_.push(std::make_pair(FenceValue, Allocator));
}
