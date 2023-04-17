#pragma once

#include "pch.h"
#include "utility.h"
#include <mutex>

/*
===============================================================================
Heap�������Ϊһ�����飬Heap�����;�����ÿ��Ԫ�صĴ�С�����⻹Ҫָ������ĳ��ȣ�
Hanlde�������Ϊ������ĳ��Ԫ�ص�ָ�룻
Handle��CPU��GPU�����ǵĵ�ַ�ǲ�һ���ģ�

FIXME: ����static��Ա�Ƿ���Ըĳɷ�static�ģ�
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

	descriptor_allocator(descriptor_allocator const&) = delete;

private:
	static std::mutex mutex_;
	static std::vector<ComPtr<ID3D12DescriptorHeap>> heap_pool_;

	D3D12_DESCRIPTOR_HEAP_TYPE heap_type_;
	ID3D12DescriptorHeap* current_heap_;
	D3D12_CPU_DESCRIPTOR_HANDLE current_handle_;
	uint32_t free_handles_;
	uint32_t descriptor_size_;
};
