#pragma once

#include "pch.h"
#include <vector>
#include <queue>
#include <mutex>

/*
===============================================================================
ID3D12CommandAllocator对象的缓存池
===============================================================================
*/
class command_allocator_pool
{
public:
	command_allocator_pool(D3D12_COMMAND_LIST_TYPE type);
	~command_allocator_pool();

	void create(ID3D12Device* device);
	void shutdown();

	ID3D12CommandAllocator* request_allocator(uint64_t completed_fence_value);
	void discard_allocator(uint64_t fence_value, ID3D12CommandAllocator* allocator);

	inline size_t size() const { return allocator_pool_.size(); }

private:
	const D3D12_COMMAND_LIST_TYPE type_;
	ID3D12Device* device_;
	/** 所有的 */
	std::vector<ID3D12CommandAllocator*> allocator_pool_;
	/** 使用完的，是上面的子集 */
	std::queue<std::pair<uint64_t, ID3D12CommandAllocator*>> ready_allocators_;
	std::mutex allocator_mutex_;
};
