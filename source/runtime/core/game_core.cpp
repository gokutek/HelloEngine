#include "game_core.h"
#include "display.h"
#include "utility.h"
#include "graphics_core.h"

namespace game_core
{
    HWND g_hWnd = nullptr;

	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static void initialize_application(game_app& game);
	static void terminate_application(game_app& game);
	static bool update_application(game_app& game);
    
	int run_application(game_app& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow)
	{
		//FIXME: 为什么编译不过？
#if 0
		if (!XMVerifyCPUSupport())
		{
			return 1;
		}
#endif
    
        //TODO:

        // 注册窗口类
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

        // 创建窗口
        RECT rc = { 0, 0, (LONG)graphics::g_DisplayWidth, (LONG)graphics::g_DisplayHeight };
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
		graphics::shutdown();
		return 0;
	}

    /** 窗口过程 */
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

	static void initialize_application(game_app& game)
    {
    }

    static void terminate_application(game_app& game)
    {
    }

    static bool update_application(game_app& game)
    {
        return true;
    }
}
