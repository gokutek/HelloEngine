#include "core/game_core.h"

class model_viewer_app : public game_core::game_app
{
public:
	virtual void startup() override;
	virtual void cleanup() override;

	virtual void update(float DeltaTime) override;
	virtual void render_scene() override;
};
