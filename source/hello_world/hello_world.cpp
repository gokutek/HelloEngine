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

	uint32_t byte_size = uint32_t(vertex_data.size() * sizeof(vertex_attr));

	upload_buffer geo_buffer;
	geo_buffer.create(L"Geometry Upload Buffer", byte_size);
	void* uploadMem = geo_buffer.map();
	memcpy(uploadMem, &vertex_data[0], byte_size);
	geo_buffer.unmap(0);

	m_GeometryBuffer.create(L"Geometry Buffer", byte_size, 1, geo_buffer);
	m_VertexBuffer = m_GeometryBuffer.vertex_buffer_view(0, byte_size, sizeof(vertex_attr));
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
