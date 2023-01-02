#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector4.hpp"

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

template<>
JSON SerializationUtils::serializeTemplated(const Vector2& value)
{
JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
return json;
}

template<>
void SerializationUtils::deserializeTemplated(Vector2& value, const JSON& json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
}

