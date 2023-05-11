#include "hello_world.h"
#include "core/utility.h"
#include <windows.h>

static bool start_unit_test()
{
	unsigned long HighBit;
	_BitScanReverse(&HighBit, 1);
	ASSERT(HighBit == 0);

	_BitScanReverse(&HighBit, 0xff);
	ASSERT(HighBit == 7);

	//new长度为0的数组，不会崩溃。
	auto* p = new int[0];
	delete[] p;

	//32位与64位进行异或
	uint32_t v1 = 0;
	uint64_t v2 = -1;
	uint64_t x = v1 ^ v2;

	std::hash<uint32_t> h;
	size_t hv = h(123);

	return true;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	//设置当前工作目录
	TCHAR path[MAX_PATH];
	GetModuleFileName(NULL, path, MAX_PATH);
	*wcsrchr(path, '\\') = 0;
	SetCurrentDirectory(path);

	start_unit_test();

	return run_application(hello_world_app(), TEXT("hello_world"), hInstance, nCmdShow);
}
