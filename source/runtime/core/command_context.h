#pragma once

#include "pch.h"
#include "gpu_resource.h"
#include <stdint.h>
#include <string>

class command_list_manager;

class command_context
{
public:
	static command_context& begin(wchar_t const* id);

	command_context(D3D12_COMMAND_LIST_TYPE type);
	virtual ~command_context();

	uint64_t flush(bool wait_for_completion);
	uint64_t finish(bool wait_for_completion);
	
	void initialize();
	void reset();

	ID3D12GraphicsCommandList* get_command_list();
	void copy_buffer(gpu_resource& dst, gpu_resource& src);

	//TODO:几个buffer的拷贝的方法

	void flush_resource_barriers();

	void set_id(std::wstring const& id);

private:
	command_context() = default;
	command_context(const command_context&) = delete;
	command_context& operator=(const command_context&) = delete;

protected:
	D3D12_COMMAND_LIST_TYPE type_;
	std::wstring id_;
	command_list_manager* owning_manager_;
	ID3D12GraphicsCommandList* rhi_command_list_;
	ID3D12CommandAllocator* rhi_current_allocator_;

	ID3D12RootSignature* rhi_cur_graphics_root_signature_;
	ID3D12RootSignature* rhi_cur_compute_root_signature_;
	ID3D12PipelineState* rhi_cur_pipeline_state_;
};

inline void command_context::set_id(std::wstring const& id)
{
	id_ = id;
}

inline void command_context::flush_resource_barriers()
{
	//TODO:
}
