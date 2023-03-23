#include "hello_world.h"
#include "core/graphics_context.h"
#include "core/utility.h"
#include <string>
#include <memory>

static bool start_unit_test()
{
	unsigned long HighBit;
	_BitScanReverse(&HighBit, 1);
	ASSERT(HighBit == 0);

	_BitScanReverse(&HighBit, 0xff);
	ASSERT(HighBit == 7);

	//new����Ϊ0�����飬���������
	auto* p = new std::string[0];
	delete[] p;

	//32λ��64λ�������
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

	//TODO:����������
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

	//TODO:����������
}
