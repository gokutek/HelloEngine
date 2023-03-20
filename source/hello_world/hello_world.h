#include "core/game_core.h"

class hello_world_app : public game_core::game_app
{
public:
	virtual void startup() override;
	virtual void cleanup() override;

	virtual void update(float delta_time) override;
	virtual void render_scene() override;
};
