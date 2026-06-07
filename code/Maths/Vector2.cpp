#include "Maths/Vector3.hpp"
#include "Maths/Vector2.hpp"
#include "Maths/Vector4.hpp"

namespace Maths
{
Vector2::Vector2(const Vector3& other) : x(other.x), y(other.y)
{
}

Vector2::Vector2(const Vector4& other) : x(other.x), y(other.y)
{
}

f32 Vector2::angle(const Vector2& v) const
{
	f32 angle = atan2f(v.y, v.x) - atan2f(this->y, this->x);
	return angle < 0 ? angle += 2 * MathUtils::PI : angle;
}
};
