#include "pso.h"
#include "root_signature.h"
#include "utility.h"
#include "graphics_core.h"
#include <mutex>
#include <thread>
#include <unordered_map>

pso::pso(wchar_t const* name)
	: name_(name),
	root_signature_(nullptr),
	rhi_pso_(nullptr)
{
}

pso::~pso()
{
}

void pso::set_root_signature(root_signature const* signature)
{
	root_signature_ = signature;
}

root_signature const* pso::get_root_signature() const
{
	return root_signature_;
}

ID3D12PipelineState* pso::get_pipeline_state_object() const
{
	return rhi_pso_;
}
