#pragma once

#include <cmath> /* ceil, max */
#include <algorithm>
#include "Core/BasicTypes.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Geometry.hpp"
#include <limits>

class MathUtils
{
public:
	static f32 FLOAT_EPSILON;

	static f32 PI;
	static f32 PI_2;
	static f32 PI_4;
	static f32 INV_PI;
	static f32 INV_2_PI;
	static f32 INV_2_SQRTPI;
	static f32 SQRT2;
	static f32 INV_SQRT2;
	static f32 E;
	static f32 LOG2E;
	static f32 LOG10E;
	static f32 LN2;
	static f32 LN10;

	static f32 PI_180;
	static f32 INV_PI_180;

	static f32 rad(f32 deg);
	static f32 deg(f32 rad);

	// f32 comparison

	static bool eqf(f32 a, f32 b, f32 epsilon);
	static bool eqf(f32 a, f32 b); // default epsilon

	static f32 clamp(f32 n, f32 lower, f32 upper);
};
