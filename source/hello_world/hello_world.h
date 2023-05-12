#pragma once

#include "core/game_core.h"
#include "core/byte_address_buffer.h"
#include "core/graphics_pso.h"
#include "model/renderer.h"
#include <DirectXMath.h>
#include <memory>

/**
 * @brief 
 */
class hello_world_app : public game_module
{
public:
	virtual void startup() override;
	virtual void cleanup() override;

	virtual void update(float delta_time) override;
	virtual void render_scene() override;

private:
	renderer renderer_;
	byte_address_buffer vertex_buffer_;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;
	uint32_t vertex_count_;
	std::unique_ptr<graphics_pso> m_HelloWorldPSO;
};
