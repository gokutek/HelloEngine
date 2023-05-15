#pragma once

#include "vector.h"
#include "quaternion.h"

/**
 * @brief TODO:
 */

class transform
{

private:
	vector3 scale_;
	vector3 translation_;
	quaternion rotation_;
};
