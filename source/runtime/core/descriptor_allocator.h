#pragma once

#include "pch.h"
#include "utility.h"
#include <mutex>

/*
===============================================================================
FIXME: 几个static成员是否可以改成非static的？
===============================================================================
*/
class descriptor_allocator
{
public:
	descriptor_allocator(D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

	D3D12_CPU_DESCRIPTOR_HANDLE allocate(uint32_t count);
	static void destroy_all();

private:
	static ID3D12DescriptorHeap* request_new_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

private:
	static std::mutex mutex_;
	static std::vector<ComPtr<ID3D12DescriptorHeap>> heap_pool_;

	D3D12_DESCRIPTOR_HEAP_TYPE heap_type_;
	ID3D12DescriptorHeap* current_heap_;
	D3D12_CPU_DESCRIPTOR_HANDLE current_handle_;
	uint32_t free_handles_;
	uint32_t descriptor_size_;
};
