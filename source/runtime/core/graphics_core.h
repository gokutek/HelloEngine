#pragma once

#include "buffer_manager.h"
#include "command_list_manager.h"
#include "command_context_manager.h"
#include "descriptor_heap.h"
#include "descriptor_allocator.h"
#include "display.h"
#include <memory>
#include <vector>

class graphics
{
public:
	void initialize(bool RequireDXRSupport = false);
	void shutdown();

private:
	int init_device(bool RequireDXRSupport);
	int init_info_queue();
	int check_feature_support();

public:
	ID3D12Device* device;
	command_list_manager command_manager_;
	command_context_manager context_manager_;
	buffer_manager buffer_manager_;
	display display_;
	std::vector<descriptor_allocator> descriptor_allocators;
};

graphics* get_rhi();
