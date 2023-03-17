#include <windows.h>
#include "model_viewer.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPWSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	return game_core::run_application(model_viewer_app(), TEXT("model_viewer_app"), hInstance, nCmdShow);
}
