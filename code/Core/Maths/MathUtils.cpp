#include "Core/Maths/MathUtils.hpp"


f32 MathUtils::rad(f32 deg)
{
	return deg * PI_180;
}

f32 MathUtils::deg(f32 rad)
{
	return rad * INV_PI_180;
}

bool MathUtils::eqf(f32 a, f32 b, f32 epsilon)
{
	return fabsf(a - b) < epsilon;
}

bool MathUtils::eqf(f32 a, f32 b)
{
	return eqf(a, b, FLOAT_EPSILON);
}

f32 MathUtils::clamp(f32 n, f32 lower, f32 upper)
{
	return std::max(lower, std::min(n, upper));
}

f32 MathUtils::det(f32 a, f32 b, f32 c, f32 d)
{
	return a * d - b * c;
}
