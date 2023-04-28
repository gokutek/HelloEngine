#include "dynamic_descriptor_heap.h"
#include "root_signature.h"
#include "descriptor_heap.h"

struct descriptor_table_cache
{
	descriptor_table_cache()
	{
		ZeroMemory(this, sizeof(*this));
	}

	uint32_t assigned_handles_bitmap; ///< 已分配标志位
	D3D12_CPU_DESCRIPTOR_HANDLE table_start; ///< 起始地址
	uint32_t table_size; ///< 长度
};

class descriptor_handle_cache
{
public:
	/**
	 * @brief 
	 */
	void clear_cache();

	/**
	 * @brief
	 */
	void unbind_all_valid();

	/**
	 * @brief
	 */
	void stage_descriptor_handles(UINT root_index, UINT offset, UINT num_handles, D3D12_CPU_DESCRIPTOR_HANDLE const* handles);

	/**
	 * @brief
	 */
	void parse_root_signature(D3D12_DESCRIPTOR_HEAP_TYPE type, root_signature const& root_sig);

	/**
	 * @brief
	 */
	uint32_t compute_staged_size();

	/**
	 * @brief
	 */
	void copy_and_bind_stale_tables(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t DescriptorSize, descriptor_handle DestHandleStart, ID3D12GraphicsCommandList* CmdList,
		void (STDMETHODCALLTYPE ID3D12GraphicsCommandList::* SetFunc)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE));

private:
	static constexpr int kMaxRootDescriptorTableSize = 16;
	static constexpr int kMaxDescriptorSize = 256;

	descriptor_table_cache root_descriptor_table_[kMaxRootDescriptorTableSize];
	uint32_t root_descriptor_tables_bitmap_;
	D3D12_CPU_DESCRIPTOR_HANDLE handle_cache_[111];
};
