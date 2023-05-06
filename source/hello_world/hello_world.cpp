#include "hello_world.h"
#include "core/graphics_context.h"
#include "core/utility.h"
#include "core/graphics_core.h"
#include "core/byte_address_buffer.h"
#include "core/upload_buffer.h"
#include "cso/hello_world_vs.h"
#include "cso/hello_world_ps.h"
#include <string>
#include <memory>

namespace test
{
	struct mesh_vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	__declspec(align(256)) struct MeshConstants
	{
		DirectX::XMMATRIX World;         // Object to world
		DirectX::XMMATRIX WorldIT;       // Object normal to world normal
	};

	/**
	 * @brief 参考Model和ModelInstance
	 */
	class model
	{
	public:
		void render();

		uint32_t m_NumNodes;
	};

	/**
	 * @brief 参考Model和ModelInstance
	 */
	class model_instance
	{
	public:
		model_instance(std::shared_ptr<const model> sourceModel)
		{
			m_MeshConstantsCPU.create(L"Mesh Constant Upload Buffer", sourceModel->m_NumNodes * sizeof(MeshConstants));
			m_MeshConstantsGPU.create(L"Mesh Constant GPU Buffer", sourceModel->m_NumNodes, sizeof(MeshConstants));
		}

		upload_buffer m_MeshConstantsCPU;
		byte_address_buffer m_MeshConstantsGPU;
	};

	static std::shared_ptr<model> load_static_mesh()
	{
		std::shared_ptr<model> mesh(new model());
		return mesh;
	}
}

void hello_world_app::startup()
{
	//TODO:定义三角形
}

void hello_world_app::cleanup()
{
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

	//TODO:绘制三角形

	context->transition_resource(*back_buffer, D3D12_RESOURCE_STATE_PRESENT, false);

	context->finish(true);
}
