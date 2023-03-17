#pragma once

#include "pch.h"
#include "command_allocator_pool.h"
#include <mutex>

class command_queue
{
public:
	command_queue(D3D12_COMMAND_LIST_TYPE Type);
	~command_queue();

	void create(ID3D12Device* device);
	void shutdown();

private:
	ID3D12CommandQueue* rhi_command_queue_;
	const D3D12_COMMAND_LIST_TYPE type_;
	command_allocator_pool allocator_pool_;
	std::mutex fence_mutex_;
	std::mutex event_mutex_;

	ID3D12Fence* fence_;
	uint64_t next_fence_value_;
	uint64_t last_completed_fence_value_;
	HANDLE fence_event_handle_;
};
