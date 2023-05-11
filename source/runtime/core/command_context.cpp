#include "command_context.h"
#include "command_context_manager.h"
#include "graphics_core.h"
#include "utility.h"
#include "math/common.h"

command_context::command_context(D3D12_COMMAND_LIST_TYPE type) 
	: type_(type),
	owning_manager_(nullptr),
	rhi_command_list_(nullptr),
	rhi_current_allocator_(nullptr),
	rhi_cur_graphics_root_signature_(nullptr),
	rhi_cur_compute_root_signature_(nullptr),
	rhi_cur_pipeline_state_(nullptr),
	dynamic_view_descriptor_heap_(this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
	dynamic_sampler_descriptor_heap_(this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER),
	num_barriers_to_flush_(0),
	cpu_linear_allocator_(linear_allocator_type::CpuWritable),
	gpu_linear_allocator_(linear_allocator_type::GpuExclusive)
{
	ZeroMemory(resource_barrier_buffers_, sizeof(resource_barrier_buffers_));
	ZeroMemory(current_descriptor_heaps_, sizeof(current_descriptor_heaps_));
}

command_context::~command_context()
{
}

uint64_t command_context::flush(bool wait_for_completion)
{
	flush_resource_barriers();

	ASSERT(rhi_current_allocator_ != nullptr);

	uint64_t fence_value = get_rhi()->command_manager_.get_queue(type_).execute_command_list(rhi_command_list_);

	if (wait_for_completion)
	{
		get_rhi()->command_manager_.wait_for_fence(fence_value);
	}

	rhi_command_list_->Reset(rhi_current_allocator_, nullptr);

	if (rhi_cur_graphics_root_signature_)
	{
		rhi_command_list_->SetGraphicsRootSignature(rhi_cur_graphics_root_signature_);
	}
	
	if (rhi_cur_compute_root_signature_)
	{
		rhi_command_list_->SetComputeRootSignature(rhi_cur_compute_root_signature_);
	}

	if (rhi_cur_pipeline_state_)
	{
		rhi_command_list_->SetPipelineState(rhi_cur_pipeline_state_);
	}

	bind_descriptor_heaps();

	return fence_value;
}

uint64_t command_context::finish(bool wait_for_completion)
{
    ASSERT(type_ == D3D12_COMMAND_LIST_TYPE_DIRECT || type_ == D3D12_COMMAND_LIST_TYPE_COMPUTE);

    flush_resource_barriers();

    ASSERT(rhi_current_allocator_ != nullptr);

    command_queue& cmd_queue = get_rhi()->command_manager_.get_queue(type_);

    uint64_t fence_value = cmd_queue.execute_command_list(rhi_command_list_);
    cmd_queue.discard_allocator(fence_value, rhi_current_allocator_);
    rhi_current_allocator_ = nullptr;

	cpu_linear_allocator_.cleanup_used_pages(fence_value);
	gpu_linear_allocator_.cleanup_used_pages(fence_value);
	dynamic_view_descriptor_heap_.cleanup_used_heaps(fence_value);
	dynamic_sampler_descriptor_heap_.cleanup_used_heaps(fence_value);

    if (wait_for_completion)
    {
        get_rhi()->command_manager_.wait_for_fence(fence_value);
    }

    get_rhi()->context_manager_.free_context(this);

    return fence_value;
}

void command_context::initialize()
{
	get_rhi()->command_manager_.create_new_command_list(type_, &rhi_command_list_, &rhi_current_allocator_);
}

void command_context::reset()
{
	// We only call Reset() on previously freed contexts.  The command list persists, but we must
	// request a new allocator.
	ASSERT(rhi_command_list_ != nullptr && rhi_current_allocator_ == nullptr);
	rhi_current_allocator_ = get_rhi()->command_manager_.get_queue(type_).request_allocator();
	rhi_command_list_->Reset(rhi_current_allocator_, nullptr);

	rhi_cur_graphics_root_signature_ = nullptr;
	rhi_cur_compute_root_signature_ = nullptr;
	rhi_cur_pipeline_state_ = nullptr;
	num_barriers_to_flush_ = 0;

	bind_descriptor_heaps();
}

void command_context::copy_buffer(gpu_resource& dst, gpu_resource& src)
{
	transition_resource(dst, D3D12_RESOURCE_STATE_COPY_DEST, false);
	transition_resource(src, D3D12_RESOURCE_STATE_COPY_SOURCE, false);
	flush_resource_barriers();
	rhi_command_list_->CopyResource(dst.get_resource(), src.get_resource());

	/*
	===========================================================================
	关于CopyResource接口的说明：
	https://learn.microsoft.com/en-us/windows/win32/api/d3d12/nf-d3d12-id3d12graphicscommandlist-copyresource
	Must be different resources.
	Must be the same type.
	Must be the same total size (bytes).
	Must have identical dimensions (width, height, depth) or be a compatible Reinterpret Copy.
	Must have compatible DXGI formats, which means the formats must be identical or at least from the same type group.
	Can't be currently mapped.
	===========================================================================
	*/
}

void command_context::copy_buffer_region(gpu_resource& dest, size_t dest_offset, gpu_resource& src, size_t src_offset, size_t num_bytes)
{
	transition_resource(dest, D3D12_RESOURCE_STATE_COPY_DEST, false);
	transition_resource(src, D3D12_RESOURCE_STATE_COPY_SOURCE, false);
	flush_resource_barriers();
	rhi_command_list_->CopyBufferRegion(dest.get_resource(), dest_offset, src.get_resource(), src_offset, num_bytes);
}

void command_context::copy_sub_resource(gpu_resource& dest, UINT dest_sub_index, gpu_resource& src, UINT src_sub_index)
{
	assert(false && "unimpl yet!");
}

void command_context::copy_texture_region(gpu_resource& dest, UINT x, UINT y, UINT z, gpu_resource& source, RECT& rect)
{
	assert(false && "unimpl yet!");
}

void command_context::initialize_texture(gpu_resource& dest, UINT num_sub_resources, D3D12_SUBRESOURCE_DATA subdata[])
{
	assert(false && "unimpl yet!");
}

void command_context::initialize_buffer(gpu_buffer& dest, const void* data, size_t num_bytes, size_t dest_offset)
{
	// 先分配一个临时GPU Upload Buffer，将CPU数据拷贝过去
	dyn_alloc mem = reserve_upload_memory(num_bytes);
	memcpy(mem.data, data, num_bytes);

	// 再进行一次GPU到GPU的内存拷贝
	transition_resource(dest, D3D12_RESOURCE_STATE_COPY_DEST, true);
	rhi_command_list_->CopyBufferRegion(dest.get_resource(), dest_offset, mem.buffer.get_resource(), 0, num_bytes);
	transition_resource(dest, D3D12_RESOURCE_STATE_GENERIC_READ, true);
}

void command_context::initialize_buffer(gpu_buffer& dest, const upload_buffer& src, size_t src_offset, size_t num_bytes, size_t dest_offset)
{
	// 计算目标可写的字节数，防止溢出
	size_t max_bytes = std::min<size_t>(dest.get_buffer_size() - dest_offset, src.get_buffer_size() - src_offset);
	num_bytes = std::min<size_t>(max_bytes, num_bytes);

	// 设置目标的barrier、拷贝数据、再恢复目标的barrier
	transition_resource(dest, D3D12_RESOURCE_STATE_COPY_DEST, true);
	rhi_command_list_->CopyBufferRegion(dest.get_resource(), dest_offset, (ID3D12Resource*)src.get_resource(), src_offset, num_bytes);
	transition_resource(dest, D3D12_RESOURCE_STATE_GENERIC_READ, true);
}

void command_context::initialize_texture_array_slice(gpu_resource& dest, UINT slice_index, gpu_resource& src)
{
	assert(false && "unimpl yet!");
}

void command_context::write_buffer(gpu_resource& dest, size_t dest_offset, const void* data, size_t num_bytes)
{
	assert(false && "unimpl yet!");
}

void command_context::transition_resource(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate)
{
	D3D12_RESOURCE_STATES old_state = resource.get_usage_state();
	if (type_ == D3D12_COMMAND_LIST_TYPE_COMPUTE)
	{
		assert(false && "unimpl yet!");
	}

	if (new_state != old_state)
	{
		ASSERT(num_barriers_to_flush_ < _countof(resource_barrier_buffers_));
		D3D12_RESOURCE_BARRIER& barrier_desc = resource_barrier_buffers_[num_barriers_to_flush_++];
		barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_desc.Transition.pResource = resource.get_resource();
		barrier_desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier_desc.Transition.StateBefore = old_state;
		barrier_desc.Transition.StateAfter = new_state;

		if (new_state == resource.get_transitioning_state())
		{
			barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_END_ONLY;
			resource.set_transitioning_state(D3D12_RESOURCE_STATES(-1));
		}
		else
		{
			barrier_desc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		}

		resource.set_usage_state(new_state);
	}
	else if (new_state == D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
	{
		assert(false && "unimpl yet!");
	}

	if (flush_immediate || num_barriers_to_flush_ == _countof(resource_barrier_buffers_))
	{
		flush_resource_barriers();
	}
}

void command_context::begin_resource_transition(gpu_resource& resource, D3D12_RESOURCE_STATES new_state, bool flush_immediate)
{
	assert(false && "unimpl yet!");
}

void command_context::insert_uav_barrier(gpu_resource& resource, bool flush_immediate)
{
	assert(false && "unimpl yet!");
}

void command_context::insert_alias_barrier(gpu_resource& before, gpu_resource& after, bool flush_immediate)
{
	assert(false && "unimpl yet!");
}

void command_context::flush_resource_barriers()
{
	if (num_barriers_to_flush_ > 0)
	{
		rhi_command_list_->ResourceBarrier(num_barriers_to_flush_, resource_barrier_buffers_);
		num_barriers_to_flush_ = 0;
	}
}

void command_context::insert_timestamp(ID3D12QueryHeap* query_heap, uint32_t query_index)
{
	assert(false && "unimpl yet!");
}

void command_context::resolve_timestamps(ID3D12Resource* readback_heap, ID3D12QueryHeap* query_heap, uint32_t num_queries)
{
	assert(false && "unimpl yet!");
}

void command_context::pix_begin_event(wchar_t const* label)
{
	assert(false && "unimpl yet!");
}

void command_context::pix_end_event()
{
	assert(false && "unimpl yet!");
}

void command_context::pix_set_marker(wchar_t const* label)
{
	assert(false && "unimpl yet!");
}

void command_context::set_descriptor_heap(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, ID3D12DescriptorHeap* heap)
{
	if (current_descriptor_heaps_[heap_type] != heap)
	{
		current_descriptor_heaps_[heap_type] = heap;
		bind_descriptor_heaps();
	}
}

void command_context::set_descriptor_heaps(UINT heap_count, D3D12_DESCRIPTOR_HEAP_TYPE heap_types[], ID3D12DescriptorHeap* heaps[])
{
	bool update = false;

	for (uint32_t i = 0; i < heap_count; ++i)
	{
		D3D12_DESCRIPTOR_HEAP_TYPE heap_type = heap_types[i];
		ID3D12DescriptorHeap* heap = heaps[i];

		if (current_descriptor_heaps_[heap_type] != heap)
		{
			current_descriptor_heaps_[heap_type] = heap;
			update = true;
		}
	}

	if (update)
	{
		bind_descriptor_heaps();
	}
}

void command_context::set_pipeline_state(pso const& pso)
{
	ID3D12PipelineState* new_pso = pso.get_pipeline_state_object();
	if (rhi_cur_pipeline_state_ != new_pso)
	{
		rhi_cur_pipeline_state_ = new_pso;
		rhi_command_list_->SetPipelineState(rhi_cur_pipeline_state_);
	}
}

void command_context::set_predication(ID3D12Resource* Buffer, UINT64 BufferOffset, D3D12_PREDICATION_OP Op)
{
	assert(false && "unimpl yet!");
}

void command_context::bind_descriptor_heaps()
{
	ID3D12DescriptorHeap* heaps[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
	UINT count = 0;
	for (int i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		if (current_descriptor_heaps_[i])
		{
			heaps[count++] = current_descriptor_heaps_[i];
		}
	}

	if (count > 0)
	{
		rhi_command_list_->SetDescriptorHeaps(count, heaps);
	}
}
