#pragma once

class game_module
{
public:
	virtual void startup() = 0;
	virtual void cleanup() = 0;
	virtual bool is_done() { return false; }

	virtual void update(float delta_time) = 0;
	virtual void render_scene() = 0;
	virtual void render_ui() {}
};
