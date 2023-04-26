#pragma once

#include "graphics_context.h"
#include "compute_context.h"
#include <mutex>
#include <vector>
#include <queue>
#include <memory>

/**
 * @brief 管理graphics_context和compute_context等。
 * 
 * @see command_context
 * @see graphics_context
 */
class command_context_manager
{
public:
	command_context_manager();
	~command_context_manager();

	/**
	 * @brief 分配graphics_context
	 * @param id 调试用的id
	 * @return graphics_context
	 */
	graphics_context* allocate_graphics_context(wchar_t const* id);

	/**
	 * @brief 释放graphics_context到回收池
	 */
	void free_context(command_context* context);

	/**
	 * @brief 删除所有的
	 */
	void destroy_all_contexts();

private:
	/**
	 * @brief 分配指定类型的context，会优先从回收池里分配
	 * @param type DIRECT或者COMPUTE，分别用于渲染和计算
	 * @return command_context
	 */
	command_context* allocate_context(D3D12_COMMAND_LIST_TYPE type);

private:
	std::vector<std::unique_ptr<command_context> > context_pools_[4];
	std::queue<command_context*> free_pools_[4];
	std::mutex mutex_;
};

/*
===============================================================================
1. 对应原来的ContextManager；
===============================================================================
*/
