#include "game_core.h"
#include "display.h"
#include "utility.h"
#include "graphics_core.h"
#include "util/command_line_arg.h"
#include "global.h"
#include <shellapi.h>

/** ���ڹ��� */
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        //TODO:
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

static void initialize_application(game_module& game)
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    CommandLineArgs::Initialize(argc, argv);

    rhi.initialize(true);

    game.startup();
}

static void terminate_application(game_module& game)
{
    game.cleanup();
}

static bool update_application(game_module& game)
{
    //TODO: ����

    game.update(1.0f / 33);
    game.render_scene();

    //TODO: ����

    //TODO: ��ȾUI

    get_rhi()->display_.present();

    return true;
}
    
int run_application(game_module& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow)
{
	//FIXME: Ϊʲô���벻����
#if 0
	if (!XMVerifyCPUSupport())
	{
		return 1;
	}
#endif
    
    //TODO:

    // ע�ᴰ����
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);
    ASSERT(0 != RegisterClassEx(&wcex), "Unable to register a window");

    // ��������
    RECT rc = { 0, 0, 1280, 720 }; //FIXME: ��display����ȥȡ
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);

    ASSERT(g_hWnd != 0);

    initialize_application(app);

	ShowWindow(g_hWnd, nCmdShow);

	do
	{
		MSG msg = {};
		bool done = false;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
				done = true;
            }
		}

        if (done)
        {
			break;
        }
	} while (update_application(app));

	terminate_application(app);

	rhi.shutdown();

	return 0;
}

graphics* get_rhi()
{
    return &rhi;
}
