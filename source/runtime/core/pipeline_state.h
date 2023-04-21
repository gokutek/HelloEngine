#pragma once

#include "pch.h"
#include <string>

class root_signature;

class pso
{
public:
	pso(wchar_t const* name);
	virtual ~pso();

	void set_root_signature(root_signature const* signature);
	root_signature const* get_root_signature() const;
	
	ID3D12PipelineState* get_pipeline_state_object() const;

protected:
	std::wstring name_;
	root_signature const* root_signature_;
	ID3D12PipelineState* rhi_pso_;
};
