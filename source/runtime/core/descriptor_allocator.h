#pragma once

#include "pch.h"
#include "utility.h"
#include <mutex>

/**
 * @brief 
 * 与descriptor_heap相比，本类的Heap数量是可以动态增长的。当前Heap剩余空间不足时，
 * 会再分配一个Heap。
 * <pre>
 * 关于Heap的说明：
 * Heap可以理解为一个数组，Heap的类型决定了每个元素的大小，另外还要指定数组的长度。
 * Hanlde可以理解为数组中某个元素的指针。
 * Handle分CPU和GPU，它们的地址是不一样的。
 * </pre>
 *
 * FIXME: 几个static成员是否可以改成非static的？
 * 
 * @see descriptor_heap
 */
class descriptor_allocator
{
public:
	descriptor_allocator(D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

	/** 
	 * @brief 从堆中分配指定数量的描述符空间
	 * @param count 描述符数量
	 * @return 起始句柄
	 */
	D3D12_CPU_DESCRIPTOR_HANDLE allocate(uint32_t count);

	/** @brief 删除所有创建的堆 */
	static void destroy_all();

private:
	/** @brief 创建一个新堆 */
	static ID3D12DescriptorHeap* request_new_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type);

	descriptor_allocator(descriptor_allocator const&) = delete;

private:
	static std::mutex mutex_; ///< 锁
	static std::vector<ComPtr<ID3D12DescriptorHeap>> heap_pool_; ///< 已分配的堆

	D3D12_DESCRIPTOR_HEAP_TYPE heap_type_; ///< 堆类型
	ID3D12DescriptorHeap* current_heap_; ///< 最近分配的堆
	D3D12_CPU_DESCRIPTOR_HANDLE current_handle_; ///< 最近堆中的下次可分配地址
	uint32_t free_handles_; ///< 最近堆中的可分配数量
	uint32_t descriptor_size_; ///< 堆中一个元素的大小
};
