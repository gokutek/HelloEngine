#include "hello_world.h"
#include "core/graphics_context.h"
#include "core/utility.h"

static bool start_unit_test()
{
	unsigned long HighBit;
	_BitScanReverse(&HighBit, 1);
	ASSERT(HighBit == 0);

	_BitScanReverse(&HighBit, 0xff);
	ASSERT(HighBit == 7);

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
	graphics_context& context = graphics_context::begin(L"Scene Render");

	//context.clear_depth();
	//context.clear_color();
	//context.finish();

	//TODO:绘制三角形
}
