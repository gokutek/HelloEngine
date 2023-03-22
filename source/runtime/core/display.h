#pragma once

#include "color_buffer.h"
#include <stdint.h>
#include <memory>

class display
{
public:
	display();

	void initialize();
	void shutdown();
	void resize(uint32_t width , uint32_t height);
	void present();

private:
	void set_native_resolution();

private:
	uint32_t display_width_;
	uint32_t display_height_;

	std::unique_ptr<color_buffer[]> display_plane_;
	size_t current_buffer_;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> rhi_swap_chain_;
};
