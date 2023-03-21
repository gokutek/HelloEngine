#pragma once

#include "pixel_buffer.h"
#include "color.h"

class color_buffer : public pixel_buffer
{
public:
	color_buffer(color clear_color);
};
