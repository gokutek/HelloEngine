#include "hello_world.h"
#include "core/graphics_context.h"
#include "core/utility.h"
#include "core/graphics_core.h"
#include "core/byte_address_buffer.h"
#include "core/upload_buffer.h"
#include "cso/hello_world_vs.h"
#include "cso/hello_world_ps.h"
#include "model/model_obj.h"
#include <string>
#include <memory>

struct vertex_attr
{
	DirectX::XMFLOAT3 pos;
};

void hello_world_app::startup()
{
	renderer_.initialize();

	// 顶点数据
	{
		model_obj model("../asset/african_head/african_head.obj");
		std::vector<vertex_attr> vertex_data;
		for (size_t i = 0; i < model.nfaces(); ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				vertex_attr attr;
				DirectX::XMStoreFloat3(&attr.pos, model.vert(i, j));
				vertex_data.push_back(attr);
			}
		}

		vertex_count_ = (uint32_t)vertex_data.size();
		uint32_t byte_size = uint32_t(vertex_count_ * sizeof(vertex_attr));

		upload_buffer geo_buffer;
		geo_buffer.create(L"Geometry Upload Buffer", byte_size);
		void* uploadMem = geo_buffer.map();
		memcpy(uploadMem, &vertex_data[0], byte_size);
		geo_buffer.unmap(0);

		vertex_buffer_.create(L"Geometry Buffer", byte_size, 1, geo_buffer);
		vertex_buffer_view_ = vertex_buffer_.vertex_buffer_view(0, byte_size, sizeof(vertex_attr));
	}

	// PSO
	{
		// 
		D3D12_RASTERIZER_DESC rasterizer_desc;	// Counter-clockwise
		rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		rasterizer_desc.ForcedSampleCount = 0;
		rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// 
		D3D12_BLEND_DESC alphaBlend = {};
		alphaBlend.IndependentBlendEnable = FALSE;
		alphaBlend.RenderTarget[0].BlendEnable = TRUE;
		alphaBlend.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		alphaBlend.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		alphaBlend.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		alphaBlend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		alphaBlend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		alphaBlend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		alphaBlend.RenderTarget[0].RenderTargetWriteMask = 0;
		alphaBlend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		// 
		D3D12_DEPTH_STENCIL_DESC DepthStateReadWrite = {};
		DepthStateReadWrite.DepthEnable = FALSE;
		DepthStateReadWrite.StencilEnable = FALSE;

		// VS输入顶点格式
		D3D12_INPUT_ELEMENT_DESC vs_vertex_input_fmt[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		DXGI_FORMAT color_format = DXGI_FORMAT_R10G10B10A2_UNORM;// get_rhi()->buffer_manager_.scene_color_buffer->get_format();
		DXGI_FORMAT depth_format = DXGI_FORMAT_UNKNOWN;// get_rhi()->buffer_manager_.scene_depth_buffer->get_format();

		hello_world_pso_.reset(new graphics_pso(L"HelloWorldPSO"));
		hello_world_pso_->set_root_signature(renderer_.get_root_signature());
		hello_world_pso_->set_rasterizer_state(rasterizer_desc);
		hello_world_pso_->set_blend_state(alphaBlend);
		hello_world_pso_->set_depth_stencil_state(DepthStateReadWrite);
		hello_world_pso_->set_input_layout(_countof(vs_vertex_input_fmt), vs_vertex_input_fmt);
		hello_world_pso_->set_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		hello_world_pso_->set_render_target_formats(1, &color_format, depth_format);
		hello_world_pso_->set_vertex_shader(hello_world_vs_cso, sizeof(hello_world_vs_cso));
		hello_world_pso_->set_pixel_shader(hello_world_ps_cso, sizeof(hello_world_ps_cso));
		hello_world_pso_->finalize();
	}
}

void hello_world_app::cleanup()
{
	renderer_.shutdown();
}

void hello_world_app::update(float delta_time)
{
}

void hello_world_app::render_scene()
{
	D3D12_VIEWPORT viewport = {};
	viewport.Width = 1280;
	viewport.Height = 720;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT scissor = {};
	scissor.left = 0;
	scissor.top = 0;
	scissor.right = 1280;
	scissor.bottom = 720;

	graphics* rhi = get_rhi();

	color_buffer* back_buffer = rhi->display_.get_current_color_buffer();
	back_buffer->set_clear_color(color(0, 0, 1, 1));

	graphics_context* context = rhi->context_manager_.allocate_graphics_context(L"Scene Render");
	context->transition_resource(*back_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
	context->set_viewport_and_scissor(viewport, scissor);
	context->clear_color(*back_buffer, nullptr);
	context->set_render_targets(1, &back_buffer->get_rtv());
	
	// 绘制模型
	context->set_pipeline_state(*hello_world_pso_);
	context->set_root_signature(*renderer_.get_root_signature());
	context->set_primitive_topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->set_vertex_buffer(0, vertex_buffer_view_);
	context->draw(vertex_count_, 0);

	context->transition_resource(*back_buffer, D3D12_RESOURCE_STATE_PRESENT, false);

	context->finish(true);
}
