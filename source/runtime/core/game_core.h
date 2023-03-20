#include "pch.h"

namespace game_core
{
	class game_app
	{
	public:
		virtual void startup() = 0;
		virtual void cleanup() = 0;
		virtual bool is_done() { return false; }

		virtual void update(float delta_time) = 0;
		virtual void render_scene() = 0;
	};

	int run_application(game_app& app, const wchar_t* className, HINSTANCE hInst, int nCmdShow);
}
