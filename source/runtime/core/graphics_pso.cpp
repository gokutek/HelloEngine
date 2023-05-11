#include "graphics_pso.h"
#include "root_signature.h"
#include "utility.h"
#include "graphics_core.h"
#include <mutex>
#include <thread>
#include <unordered_map>

static std::unordered_map<size_t, ComPtr<ID3D12PipelineState> > s_graphics_pso_hashmap;

graphics_pso::graphics_pso(wchar_t const* name)
	: pso(name)
{
	ZeroMemory(&state_desc_, sizeof(state_desc_));
	state_desc_.NodeMask = 1;
	state_desc_.SampleMask = 0xFFFFFFFFu;
	state_desc_.SampleDesc.Count = 1;
	state_desc_.InputLayout.NumElements = 0;
}

void graphics_pso::set_blend_state(D3D12_BLEND_DESC const& blend_desc)
{
	state_desc_.BlendState = blend_desc;
}

void graphics_pso::set_rasterizer_state(D3D12_RASTERIZER_DESC const& rasterizer_desc)
{
	state_desc_.RasterizerState = rasterizer_desc;
}

void graphics_pso::set_depthstencil_state(D3D12_DEPTH_STENCIL_DESC const& depthstencil_desc)
{
	state_desc_.DepthStencilState = depthstencil_desc;
}

void graphics_pso::set_sample_mask(UINT sample_mask)
{
	state_desc_.SampleMask = sample_mask;
}

void graphics_pso::set_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology_type)
{
	state_desc_.PrimitiveTopologyType = topology_type;
}

void graphics_pso::set_depth_target_format(DXGI_FORMAT dsv_format, UINT msaa_count, UINT msaa_quality)
{
	set_render_target_formats(0, nullptr, dsv_format, msaa_count, msaa_quality);
}

void graphics_pso::set_render_target_format(DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format, UINT msaa_count, UINT msaa_quality)
{
	set_render_target_formats(1, &rtv_format, dsv_format, msaa_count, msaa_quality);
}

void graphics_pso::set_render_target_formats(UINT num_rtvs, DXGI_FORMAT const* rtv_formats, DXGI_FORMAT dsv_format, UINT msaa_count, UINT msaa_quality)
{
	ASSERT(num_rtvs <= 8);
	for (UINT i = 0; i < num_rtvs; ++i)
	{
		ASSERT(rtv_formats[i] != DXGI_FORMAT_UNKNOWN);
		state_desc_.RTVFormats[i] = rtv_formats[i];
	}

	for (UINT i = num_rtvs; i < state_desc_.NumRenderTargets; ++i)
	{
		state_desc_.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	}

	state_desc_.NumRenderTargets = num_rtvs;
	state_desc_.DSVFormat = dsv_format;
	state_desc_.SampleDesc.Count = msaa_count;
	state_desc_.SampleDesc.Quality = msaa_quality;
}

void graphics_pso::set_input_layout(UINT num_elements, D3D12_INPUT_ELEMENT_DESC const* input_element_descs)
{
	state_desc_.InputLayout.NumElements = num_elements;
	if (num_elements > 0)
	{
		input_layouts_.reset(new D3D12_INPUT_ELEMENT_DESC[num_elements]);
		memcpy(input_layouts_.get(), input_element_descs, sizeof(D3D12_INPUT_ELEMENT_DESC) * num_elements);
	}
	else
	{
		input_layouts_.release();
	}
}

void graphics_pso::set_primitive_restart(D3D12_INDEX_BUFFER_STRIP_CUT_VALUE value)
{
	state_desc_.IBStripCutValue = value;
}

void graphics_pso::set_vertex_shader(void const* shader_bytecode, size_t size)
{
	state_desc_.VS = CD3DX12_SHADER_BYTECODE(shader_bytecode, size);
}

void graphics_pso::set_pixel_shader(void const* shader_bytecode, size_t size)
{
	state_desc_.PS = CD3DX12_SHADER_BYTECODE(shader_bytecode, size);
}

void graphics_pso::set_geometry_shader(void const* shader_bytecode, size_t size)
{
	state_desc_.GS = CD3DX12_SHADER_BYTECODE(shader_bytecode, size);
}

void graphics_pso::set_hull_shader(void const* shader_bytecode, size_t size)
{
	state_desc_.HS = CD3DX12_SHADER_BYTECODE(shader_bytecode, size);
}

void graphics_pso::set_domain_shader(void const* shader_bytecode, size_t size)
{
	state_desc_.DS = CD3DX12_SHADER_BYTECODE(shader_bytecode, size);
}

void graphics_pso::set_vertex_shader(D3D12_SHADER_BYTECODE const& shader_bytecode)
{
	state_desc_.VS = shader_bytecode;
}

void graphics_pso::set_pixel_shader(D3D12_SHADER_BYTECODE const& shader_bytecode)
{
	state_desc_.PS = shader_bytecode;
}

void graphics_pso::set_geometry_shader(D3D12_SHADER_BYTECODE const& shader_bytecode)
{
	state_desc_.GS = shader_bytecode;
}

void graphics_pso::set_hull_shader(D3D12_SHADER_BYTECODE const& shader_bytecode)
{
	state_desc_.HS = shader_bytecode;
}

void graphics_pso::set_domain_shader(D3D12_SHADER_BYTECODE const& shader_bytecode)
{
	state_desc_.DS = shader_bytecode;
}

int graphics_pso::finalize()
{
	//确保RootSignature已创建完成
	state_desc_.pRootSignature = root_signature_->get_signature();
	ASSERT(state_desc_.pRootSignature);

	//计算结构体的哈希值
	state_desc_.InputLayout.pInputElementDescs = nullptr;
	size_t hash_code = utility::hash_array(&state_desc_, 1);
	hash_code = utility::hash_array(input_layouts_.get(), state_desc_.InputLayout.NumElements, hash_code);
	state_desc_.InputLayout.pInputElementDescs = input_layouts_.get();

	ID3D12PipelineState** PSORef = nullptr;
	bool firstCompile = false;

	{
		static std::mutex mutex_graphics_pso_map;
		std::lock_guard<std::mutex> guard(mutex_graphics_pso_map);
		auto iter = s_graphics_pso_hashmap.find(hash_code);

		if (iter == s_graphics_pso_hashmap.end())
		{
			firstCompile = true;
			PSORef = s_graphics_pso_hashmap[hash_code].GetAddressOf(); //这里先在map里插入占坑
		}
		else
		{
			PSORef = iter->second.GetAddressOf();
		}
	}

	//假设有两个线程同时调用本函数，且PSO的哈希值相同，一个线程将进入if分支，另一线程进入else分支

	if (firstCompile)
	{
		ASSERT(state_desc_.DepthStencilState.DepthEnable != (state_desc_.DSVFormat == DXGI_FORMAT_UNKNOWN));
		ASSERT_SUCCEEDED(get_rhi()->device->CreateGraphicsPipelineState(&state_desc_, IID_PPV_ARGS(&rhi_pso_)));
		s_graphics_pso_hashmap[hash_code].Attach(rhi_pso_); //这里访问hashmap不需要加锁
		rhi_pso_->SetName(name_.c_str());
	}
	else
	{
		while (*PSORef == nullptr)
		{
			std::this_thread::yield();
		}

		rhi_pso_ = *PSORef;
	}

	return 0;
}
