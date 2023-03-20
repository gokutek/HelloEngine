#pragma once

#include "command_queue.h"

class command_list_manager
{
public:
	command_list_manager();
	~command_list_manager();

	void create(ID3D12Device* device);
	void shutdown();

	command_queue& get_graphics_queue();
	command_queue& get_compute_queue();
	command_queue& get_copy_queue();

	command_queue& get_queue(D3D12_COMMAND_LIST_TYPE type);

	//TODO:GetCommandQueue

	void create_new_command_list(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator);
	bool is_fence_complete(uint64_t fence_value);
	void wait_for_fence(uint64_t fence_value);

	void idle_gpu();

private:
	ID3D12Device* device_;
	command_queue graphics_queue_;
	command_queue compute_queue_;
	command_queue copy_queue_;
};

inline command_queue& command_list_manager::get_graphics_queue()
{
	return graphics_queue_;
}

inline command_queue& command_list_manager::get_compute_queue()
{
	return compute_queue_;
}

inline command_queue& command_list_manager::get_copy_queue()
{
	return copy_queue_;
}

inline command_queue& command_list_manager::get_queue(D3D12_COMMAND_LIST_TYPE type)
{
	switch (type)
	{

	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		return compute_queue_;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		return copy_queue_;
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	default:
		return graphics_queue_;
	}
}
