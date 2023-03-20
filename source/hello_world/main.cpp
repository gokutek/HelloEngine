#include <windows.h>
#include "hello_world.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	return game_core::run_application(hello_world_app(), TEXT("hello_world"), hInstance, nCmdShow);
}
