#pragma once

#include "linear_allocation_page.h"
#include "linear_allocator.h"
#include <memory>
#include <mutex>
#include <queue>

 /**
  * @brief 管理已分配的GPU内存页
  */
class linear_allocator_page_manager
{
public:
	linear_allocator_page_manager(linear_allocator_type allocation_type);
	linear_allocation_page* request_page();
	linear_allocation_page* create_new_page(size_t page_size);

	void discard_pages(uint64_t fence_value, std::vector<linear_allocation_page*> const& pages);
	void free_large_pages(uint64_t fence_value, std::vector<linear_allocation_page*> const& pages);

	void destroy();

private:
	linear_allocator_type const allocation_type_;
	std::vector<std::unique_ptr<linear_allocation_page> > page_pool_; ///< 所有创建的页
	std::queue<std::pair<uint64_t, linear_allocation_page*> > retired_pages_; ///< discard的页
	std::queue<std::pair<uint64_t, linear_allocation_page*> > deletion_queue_; ///< free的页
	std::queue<linear_allocation_page*> available_pages_; ///< 可用的页
	std::mutex mutex_;
};
