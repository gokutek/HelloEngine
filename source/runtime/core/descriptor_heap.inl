#pragma once

/*
===============================================================================
===============================================================================
*/
inline descriptor_handle::descriptor_handle()
{
	cpu_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
	gpu_handle_.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
}

inline descriptor_handle::descriptor_handle(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) :
	cpu_handle_(cpu_handle),
	gpu_handle_(gpu_handle)
{
}

inline descriptor_handle descriptor_handle::operator+(int offset) const
{
	descriptor_handle ret = *this;
	ret += offset;
	return ret;
}

inline void descriptor_handle::operator+=(int offset)
{
	if (cpu_handle_.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		cpu_handle_.ptr += offset;
	}

	if (gpu_handle_.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN)
	{
		gpu_handle_.ptr += offset;
	}
}

inline descriptor_handle::operator D3D12_CPU_DESCRIPTOR_HANDLE() const
{
	return cpu_handle_;
}

inline descriptor_handle::operator D3D12_GPU_DESCRIPTOR_HANDLE() const
{
	return gpu_handle_;
}

inline size_t descriptor_handle::get_cpu_ptr() const
{
	return cpu_handle_.ptr;
}

inline uint64_t descriptor_handle::get_gpu_ptr() const
{
	return gpu_handle_.ptr;
}

/*
===============================================================================
===============================================================================
*/

inline bool descriptor_heap::has_available_space(uint32_t count)
{
	return count <= free_descriptors_num_;
}

inline descriptor_handle descriptor_heap::operator[](uint32_t index) const
{
	return first_handle_ + index * descriptor_size_;
}

inline uint32_t descriptor_heap::get_offset_of_handle(const descriptor_handle& handle)
{
	return uint32_t((handle.get_cpu_ptr() - first_handle_.get_cpu_ptr()) / descriptor_size_);
}

inline ID3D12DescriptorHeap* descriptor_heap::get_heap_pointer() const
{
	return rhi_heap_.Get();
}

inline uint32_t descriptor_heap::get_descriptor_size() const
{
	return descriptor_size_;
}
