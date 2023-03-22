#pragma once

#include "pch.h"
#include <stdint.h>

/*
===============================================================================
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
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rhi_heap_;
	D3D12_DESCRIPTOR_HEAP_DESC heap_desc_;
	uint32_t descriptor_size_;
	uint32_t free_descriptors_num_;
	descriptor_handle first_handle_;
	descriptor_handle next_free_handle_;
};

#include "descriptor_heap.inl"
