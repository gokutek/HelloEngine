#pragma once

#include "pch.h"
#include <memory>
#include <stdint.h>

class root_parameter;

/*
===============================================================================
===============================================================================
*/
class root_signature
{
public:
	root_signature(uint32_t root_params_num, uint32_t static_samplers_num);
	~root_signature();

	void destroy_all();
	void reset(uint32_t root_params_num, uint32_t static_samplers_num);
	root_parameter* get_root_parameter(size_t index);
	void init_static_sampler(uint32_t register_id, D3D12_SAMPLER_DESC const& desc, D3D12_SHADER_VISIBILITY visibility);
	void finalize(wchar_t const* name, D3D12_ROOT_SIGNATURE_FLAGS flags);

private:
	bool finalized_;
	uint32_t parameters_num_;
	uint32_t samplers_num_;
	uint32_t init_static_samplers_num_;
	uint32_t descriptor_table_bmp_;
	uint32_t sampler_table_bmp_;
	uint32_t descriptor_table_size_[16];

	std::unique_ptr<root_parameter[]> param_array_;
	std::unique_ptr<D3D12_STATIC_SAMPLER_DESC[]> sampler_array_;
	ID3D12RootSignature* rhi_root_signature_;
};
