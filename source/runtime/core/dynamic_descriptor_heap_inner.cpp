#include "dynamic_descriptor_heap_inner.h"
#include "utility.h"
#include "graphics_core.h"

void descriptor_handle_cache::clear_cache()
{
	root_descriptor_tables_bitmap_ = 0;
	stale_root_params_bitmap_ = 0;
	max_cached_descriptors_ = 0;
}

void descriptor_handle_cache::unbind_all_valid()
{
}

void descriptor_handle_cache::stage_descriptor_handles(UINT root_index, UINT offset, UINT num_handles, D3D12_CPU_DESCRIPTOR_HANDLE const* handles)
{
    ASSERT(((1 << root_index) & root_descriptor_tables_bitmap_) != 0, "Root parameter is not a CBV_SRV_UAV descriptor table");
    ASSERT(offset + num_handles <= root_descriptor_table_[root_index].table_size);

    descriptor_table_cache& table_cache = root_descriptor_table_[root_index];
    D3D12_CPU_DESCRIPTOR_HANDLE* CopyDest = table_cache.table_start + offset;
    for (UINT i = 0; i < num_handles; ++i)
    {
        CopyDest[i] = handles[i];
    }
    table_cache.assigned_handles_bitmap |= ((1 << num_handles) - 1) << offset;
    stale_root_params_bitmap_ |= (1 << root_index);
}

void descriptor_handle_cache::parse_root_signature(D3D12_DESCRIPTOR_HEAP_TYPE type, root_signature const& root_sig)
{
    UINT current_offset = 0;

    ASSERT(root_sig.get_parameters_num() <= kMaxNumDescriptorTables, "Maybe we need to support something greater");

    stale_root_params_bitmap_ = 0;
    root_descriptor_tables_bitmap_ = (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? root_sig.get_sampler_table_bmp() : root_sig.get_descriptor_table_bmp());

    // 扫描每一位，处理为1的比特位
    unsigned long bitmap = root_descriptor_tables_bitmap_;
    unsigned long index;
    while (_BitScanForward(&index, bitmap))
    {
        bitmap ^= (1 << index); // 将该位置为0

        UINT table_size = root_sig.get_descriptor_table_size()[index];
        ASSERT(table_size > 0);

        descriptor_table_cache& RootDescriptorTable = root_descriptor_table_[index];
        RootDescriptorTable.assigned_handles_bitmap = 0;
        RootDescriptorTable.table_start = handle_cache_ + current_offset;
        RootDescriptorTable.table_size = table_size;

        current_offset += table_size;
    }

    max_cached_descriptors_ = current_offset;

    ASSERT(max_cached_descriptors_ <= kMaxNumDescriptors, "Exceeded user-supplied maximum cache size");
}

uint32_t descriptor_handle_cache::compute_staged_size()
{
	return 0;
}

void descriptor_handle_cache::copy_and_bind_stale_tables(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptor_size, descriptor_handle dest_handle_start, ID3D12GraphicsCommandList* cmd_list, void(STDMETHODCALLTYPE ID3D12GraphicsCommandList::* setter)(UINT, D3D12_GPU_DESCRIPTOR_HANDLE))
{
    uint32_t StaleParamCount = 0;
    uint32_t TableSize[kMaxNumDescriptorTables];
    uint32_t RootIndices[kMaxNumDescriptorTables];
    uint32_t NeededSpace = 0;
    uint32_t root_index;

    // Sum the maximum assigned offsets of stale descriptor tables to determine total needed space.
    uint32_t StaleParams = stale_root_params_bitmap_;
    while (_BitScanForward((unsigned long*)&root_index, StaleParams))
    {
        RootIndices[StaleParamCount] = root_index;
        StaleParams ^= (1 << root_index);

        uint32_t MaxSetHandle;
        ASSERT(TRUE == _BitScanReverse((unsigned long*)&MaxSetHandle, root_descriptor_table_[root_index].assigned_handles_bitmap),
            "Root entry marked as stale but has no stale descriptors");

        NeededSpace += MaxSetHandle + 1;
        TableSize[StaleParamCount] = MaxSetHandle + 1;

        ++StaleParamCount;
    }

    ASSERT(StaleParamCount <= kMaxNumDescriptorTables, "We're only equipped to handle so many descriptor tables");

    stale_root_params_bitmap_ = 0;

    static const uint32_t kMaxDescriptorsPerCopy = 16;
    UINT NumDestDescriptorRanges = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE pDestDescriptorRangeStarts[kMaxDescriptorsPerCopy];
    UINT pDestDescriptorRangeSizes[kMaxDescriptorsPerCopy];

    UINT NumSrcDescriptorRanges = 0;
    D3D12_CPU_DESCRIPTOR_HANDLE pSrcDescriptorRangeStarts[kMaxDescriptorsPerCopy];
    UINT pSrcDescriptorRangeSizes[kMaxDescriptorsPerCopy];

    for (uint32_t i = 0; i < StaleParamCount; ++i)
    {
        root_index = RootIndices[i];
        (cmd_list->*setter)(root_index, dest_handle_start);

        descriptor_table_cache& RootDescTable = root_descriptor_table_[root_index];

        D3D12_CPU_DESCRIPTOR_HANDLE* SrcHandles = RootDescTable.table_start;
        uint64_t SetHandles = (uint64_t)RootDescTable.assigned_handles_bitmap;
        D3D12_CPU_DESCRIPTOR_HANDLE CurDest = dest_handle_start;
        dest_handle_start += TableSize[i] * descriptor_size;

        unsigned long SkipCount;
        while (_BitScanForward64(&SkipCount, SetHandles))
        {
            // Skip over unset descriptor handles
            SetHandles >>= SkipCount;
            SrcHandles += SkipCount;
            CurDest.ptr += SkipCount * descriptor_size;

            unsigned long DescriptorCount;
            _BitScanForward64(&DescriptorCount, ~SetHandles);
            SetHandles >>= DescriptorCount;

            // If we run out of temp room, copy what we've got so far
            if (NumSrcDescriptorRanges + DescriptorCount > kMaxDescriptorsPerCopy)
            {
                get_rhi()->device->CopyDescriptors(
                    NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
                    NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
                    type);

                NumSrcDescriptorRanges = 0;
                NumDestDescriptorRanges = 0;
            }

            // Setup destination range
            pDestDescriptorRangeStarts[NumDestDescriptorRanges] = CurDest;
            pDestDescriptorRangeSizes[NumDestDescriptorRanges] = DescriptorCount;
            ++NumDestDescriptorRanges;

            // Setup source ranges (one descriptor each because we don't assume they are contiguous)
            for (uint32_t j = 0; j < DescriptorCount; ++j)
            {
                pSrcDescriptorRangeStarts[NumSrcDescriptorRanges] = SrcHandles[j];
                pSrcDescriptorRangeSizes[NumSrcDescriptorRanges] = 1;
                ++NumSrcDescriptorRanges;
            }

            // Move the destination pointer forward by the number of descriptors we will copy
            SrcHandles += DescriptorCount;
            CurDest.ptr += DescriptorCount * descriptor_size;
        }
    }

    get_rhi()->device->CopyDescriptors(
        NumDestDescriptorRanges, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes,
        NumSrcDescriptorRanges, pSrcDescriptorRangeStarts, pSrcDescriptorRangeSizes,
        type);
}
