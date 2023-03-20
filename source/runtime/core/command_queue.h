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
	bool is_ready() const;

	uint64_t increment_fence();
	bool is_fence_complete(uint64_t fence_value);
	void stall_for_fence(uint64_t fence_value);
	void stall_for_producer(command_queue& producer);
	void wait_for_fence(uint64_t fence_value);
	void wait_for_idle();

	ID3D12CommandQueue* get_command_queue();
	uint64_t get_next_fence_value() const;

	ID3D12Fence* get_fence();

	uint64_t execute_command_list(ID3D12CommandList* list);
	ID3D12CommandAllocator* request_allocator();
	void discard_allocator(uint64_t fence_value_for_reset, ID3D12CommandAllocator* allocator);

private:
	const D3D12_COMMAND_LIST_TYPE type_;
	ID3D12CommandQueue* rhi_command_queue_;
	command_allocator_pool allocator_pool_;
	std::mutex fence_mutex_;
	std::mutex event_mutex_;

	ID3D12Fence* fence_;
	uint64_t next_fence_value_;
	uint64_t last_completed_fence_value_;
	HANDLE fence_event_handle_;
};

inline bool command_queue::is_ready() const
{
	return rhi_command_queue_ != nullptr;
}

inline void command_queue::wait_for_idle()
{
	wait_for_fence(increment_fence());
}

inline ID3D12CommandQueue* command_queue::get_command_queue()
{
	return rhi_command_queue_;
}

inline uint64_t command_queue::get_next_fence_value() const
{
	return next_fence_value_;
}

inline ID3D12Fence* command_queue::get_fence()
{
	return fence_;
}

/*
===============================================================================
关于fence的值：
高8位用来存储类型；
===============================================================================
*/
