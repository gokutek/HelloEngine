#pragma once

#include "core/pch.h"
#include <memory>

class graphics_pso;
class root_signature;

/*
===============================================================================
===============================================================================
*/
class renderer
{
public:
	int initialize();
	void shutdown();

private:
	void initialize_common_state();
	void init_with_default_pso(graphics_pso* pso);

private:
	D3D12_RASTERIZER_DESC rasterizer_default_; // Counter-clockwise

	D3D12_DEPTH_STENCIL_DESC depth_state_disabled_;
	D3D12_DEPTH_STENCIL_DESC depth_state_read_write_;

	D3D12_BLEND_DESC blend_disable_;

	std::unique_ptr<root_signature> root_signature_;

	std::unique_ptr<graphics_pso> default_pso_;
	std::unique_ptr<graphics_pso> skybox_pso_;
};
