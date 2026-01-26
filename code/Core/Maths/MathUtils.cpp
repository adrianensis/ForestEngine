#include "Core/Maths/MathUtils.hpp"

NS_BEGIN(Maths)
Core::f32 MathUtils::rad(Core::f32 deg)
{
	return deg * PI_180;
}

Core::f32 MathUtils::deg(Core::f32 rad)
{
	return rad * INV_PI_180;
}

bool MathUtils::eqf(Core::f32 a, Core::f32 b, Core::f32 epsilon)
{
	return fabsf(a - b) < epsilon;
}

bool MathUtils::eqf(Core::f32 a, Core::f32 b)
{
	return eqf(a, b, FLOAT_EPSILON);
}

Core::f32 MathUtils::clamp(Core::f32 n, Core::f32 lower, Core::f32 upper)
{
	return std::max(lower, std::min(n, upper));
}

Core::f32 MathUtils::det(Core::f32 a, Core::f32 b, Core::f32 c, Core::f32 d)
{
	return a * d - b * c;
}

Core::f32 MathUtils::lerp(Core::f32 start, Core::f32 end, Core::f32 t)
{
    return (1.0f - t) * start + t * end;
}
NS_END