#include "renderer.h"
#include "core/pipeline_state.h"
#include "core/root_signature.h"
#include "core/root_parameter.h"
#include "core/graphics_core.h"
#include "cso/skybox_vs.h"
#include "cso/skybox_ps.h"
#include "cso/hello_vs.h"
#include "cso/hello_ps.h"

void renderer::initialize_common_state()
{
    //光栅化默认方式
    rasterizer_default_.FillMode = D3D12_FILL_MODE_SOLID;
    rasterizer_default_.CullMode = D3D12_CULL_MODE_BACK;
    rasterizer_default_.FrontCounterClockwise = TRUE;
    rasterizer_default_.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizer_default_.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rasterizer_default_.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rasterizer_default_.DepthClipEnable = TRUE;
    rasterizer_default_.MultisampleEnable = FALSE;
    rasterizer_default_.AntialiasedLineEnable = FALSE;
    rasterizer_default_.ForcedSampleCount = 0;
    rasterizer_default_.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    //
    depth_state_disabled_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depth_state_disabled_.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    depth_state_disabled_.StencilEnable = FALSE;
    depth_state_disabled_.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    depth_state_disabled_.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    depth_state_disabled_.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    depth_state_disabled_.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depth_state_disabled_.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depth_state_disabled_.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depth_state_disabled_.BackFace = depth_state_disabled_.FrontFace;

    depth_state_read_write_ = depth_state_disabled_;
    depth_state_read_write_.DepthEnable = TRUE;
    depth_state_read_write_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depth_state_read_write_.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;

    //
    D3D12_BLEND_DESC alphaBlend = {};
    alphaBlend.IndependentBlendEnable = FALSE;
    alphaBlend.RenderTarget[0].BlendEnable = FALSE;
    alphaBlend.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    alphaBlend.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    alphaBlend.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    alphaBlend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    alphaBlend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
    alphaBlend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    alphaBlend.RenderTarget[0].RenderTargetWriteMask = 0;

    alphaBlend.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blend_disable_ = alphaBlend;
}

int renderer::initialize()
{
    //初始化Root Signature
    root_signature_.reset(new root_signature(2, 0));//FIXME:
    root_signature_->get_root_parameter(0).init_as_buffer_cbv(0, D3D12_SHADER_VISIBILITY_VERTEX);
    root_signature_->get_root_parameter(1).init_as_buffer_cbv(0, D3D12_SHADER_VISIBILITY_PIXEL);
    root_signature_->finalize(L"RootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    DXGI_FORMAT color_format = get_rhi()->buffer_manager_.scene_color_buffer->get_format();
    DXGI_FORMAT depth_format = get_rhi()->buffer_manager_.scene_depth_buffer->get_format();

	//初始化默认PSO
	default_pso_.reset(new graphics_pso(L"renderer: Default PSO"));
    default_pso_->set_root_signature(root_signature_.get());
    default_pso_->set_rasterizer_state(rasterizer_default_);
    default_pso_->set_blend_state(blend_disable_);
    default_pso_->set_depthstencil_state(depth_state_read_write_);
    default_pso_->set_input_layout(0, nullptr);
    default_pso_->set_primitive_topology_type(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    default_pso_->set_render_target_formats(1, &color_format, depth_format);
    default_pso_->set_vertex_shader(hello_vs_cso, sizeof(hello_vs_cso));
    default_pso_->set_pixel_shader(hello_ps_cso, sizeof(hello_ps_cso));

	skybox_pso_.reset(new graphics_pso(L"renderer: Skybox PSO"));
	init_with_default_pso(skybox_pso_.get());

	return 0;
}

void renderer::shutdown()
{
}

void renderer::init_with_default_pso(graphics_pso* pso)
{
}
