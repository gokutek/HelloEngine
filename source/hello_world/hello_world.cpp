#include "hello_world.h"
#include "core/graphics_context.h"
#include "core/utility.h"
#include "core/graphics_core.h"
#include <string>
#include <memory>

static bool start_unit_test()
{
	unsigned long HighBit;
	_BitScanReverse(&HighBit, 1);
	ASSERT(HighBit == 0);

	_BitScanReverse(&HighBit, 0xff);
	ASSERT(HighBit == 7);

	//new长度为0的数组，不会崩溃。
	auto* p = new std::string[0];
	delete[] p;

	//32位与64位进行异或
	uint32_t v1 = 0;
	uint64_t v2 = -1;
	uint64_t x = v1 ^ v2;

	std::hash<uint32_t> h;
	size_t hv = h(123);

	return true;
}

void hello_world_app::startup()
{
	start_unit_test();

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

	color_buffer* cb = get_rhi()->display_.get_current_color_buffer();
	cb->set_clear_color(color(0, 0, 1, 1));

	graphics_context& context = graphics_context::begin(L"Scene Render");
	context.transition_resource(*cb, D3D12_RESOURCE_STATE_RENDER_TARGET, false);
	//context.set_viewport_and_scissor(viewport, scissor);
	context.clear_color(*cb, nullptr);
	//TODO:绘制三角形
	context.transition_resource(*cb, D3D12_RESOURCE_STATE_PRESENT, false);

	context.finish(true);
}
