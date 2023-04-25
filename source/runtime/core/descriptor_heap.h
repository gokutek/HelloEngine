#pragma once

#include "pch.h"
#include "utility.h"
#include <stdint.h>

/*
===============================================================================
描述符堆中的句柄，指向一个描述符。亦可理解为数组中一个元素的指针。
===============================================================================
*/
class descriptor_handle
{
public:
	descriptor_handle();
	descriptor_handle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle);

	descriptor_handle operator+(int offset) const;
	void operator+=(int offset);

	operator D3D12_CPU_DESCRIPTOR_HANDLE() const;
	operator D3D12_GPU_DESCRIPTOR_HANDLE() const;

	size_t get_cpu_ptr() const;
	uint64_t get_gpu_ptr() const;

	//TODO:

private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_;
};

/*
===============================================================================
Heap是一个数组，因此需要指定元素类型(描述符堆类型)、数组长度；
HANDLE就是一个指针；
有了堆的起始内存地址，可以求出任一元素的地址；
描述符堆类型一共有4种(各种View以及Sampler)；
===============================================================================
*/
class descriptor_heap
{
public:
	descriptor_heap();
	~descriptor_heap();

	void create(wchar_t const* debug_heap_name, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t max_count);
	void destroy();

	bool has_available_space(uint32_t count);
	descriptor_handle alloc(uint32_t count);

	descriptor_handle operator[](uint32_t index) const;

	uint32_t get_offset_of_handle(const descriptor_handle& handle);
	bool is_valid_handle(const descriptor_handle& handle) const;
	ID3D12DescriptorHeap* get_heap_pointer() const;
	uint32_t get_descriptor_size() const;

private:
	ComPtr<ID3D12DescriptorHeap> rhi_heap_;
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc_;
	uint32_t descriptor_size_; //元素大小
	uint32_t free_descriptors_num_; //未使用的描述符个数
	descriptor_handle first_handle_; //堆的起始地址
	descriptor_handle next_free_handle_; //可分配的描述符句柄
};

#include "descriptor_heap.inl"
