#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector4.hpp"

Vector3::Vector3(const Vector2& other) : x(other.x), y(other.y), z(0.0f)
{
}

Vector3::Vector3(const Vector2& other, f32 z) : x(other.x), y(other.y), z(z)
{
}

Vector3::Vector3(const Vector4& other) : x(other.x), y(other.y), z(other.z)
{
}

Vector3& Vector3::cross(const Vector3& v)
{
	f32 x1 = this->x;
	f32 y1 = this->y;
	f32 z1 = this->z;

	f32 x2 = v.x;
	f32 y2 = v.y;
	f32 z2 = v.z;

	this->x = y1 * z2 - z1 * y2;
	this->y = z1 * x2 - x1 * z2;
	this->z = x1 * y2 - y1 * x2;

	return *this;
}

Vector3& Vector3::nlerp(const Vector3& target, f32 t)
{
	this->lerp(target, t).nor();
	return *this;
}

Vector3& Vector3::slerp(const Vector3& target, f32 t)
{
	if (t == 0)
		return *this;

	f32 theta = angle(target);

	f32 sinTheta = sinf(theta);

	return this->mul(sinf((1 - t) * theta) / sinTheta).add(target * (sinf(t * theta) / sinTheta));
}

f32 Vector3::angle(const Vector3& v) const
{
	/*
	* angle is acute (positive dot product)
	* perpendicular (zero dot product)
	* or obtuse (negative dot product).
	*/
	return acosf(this->dot(v) / (this->len() * v.len()));
}

f32 Vector3::angle(const Vector3& v, const Vector3& n) const
{
	f32 dot = this->dot(v);
	Vector3 cross = Vector3(*this).cross(v);
	f32 radians = atan2f(cross.len(), dot);

	radians = n.dot(cross) < 0.0f ? (2.0f * MathUtils::PI) - radians : radians;

	return radians;
}

template<>
JSON SerializationUtils::serializeTemplated(const Vector3& value)
{
JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
SERIALIZE("z", value.z)
return json;
}

template<>
void SerializationUtils::deserializeTemplated(Vector3& value, const JSON& json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
DESERIALIZE("z", value.z)
}

