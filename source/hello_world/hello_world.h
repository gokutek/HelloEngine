#pragma once

#include "core/game_core.h"
#include "core/byte_address_buffer.h"
#include <DirectXMath.h>
#include <memory>

class hello_world_app : public game_module
{
public:
	virtual void startup() override;
	virtual void cleanup() override;

	virtual void update(float delta_time) override;
	virtual void render_scene() override;

private:
	byte_address_buffer m_GeometryBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_IndexBuffer;
};
