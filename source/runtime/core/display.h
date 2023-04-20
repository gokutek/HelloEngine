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

	uint32_t get_width() const;
	uint32_t get_height() const;

	color_buffer* get_current_color_buffer();

private:
	void set_native_resolution();

private:
	uint32_t display_width_;
	uint32_t display_height_;

	std::unique_ptr<color_buffer[]> display_plane_;
	size_t current_buffer_;
	ComPtr<IDXGISwapChain3> rhi_swap_chain_;
};

inline uint32_t display::get_width() const 
{
	return display_width_; 
}

inline uint32_t display::get_height() const 
{
	return display_height_; 
}

inline color_buffer* display::get_current_color_buffer()
{
	return &display_plane_[current_buffer_];
}
