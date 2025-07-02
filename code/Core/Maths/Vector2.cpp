#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector4.hpp"

NS_BEGIN(Maths)
Vector2::Vector2(const Vector3& other) : x(other.x), y(other.y)
{
}

Vector2::Vector2(const Vector4& other) : x(other.x), y(other.y)
{
}

Core::f32 Vector2::angle(const Vector2& v) const
{
	Core::f32 angle = atan2f(v.y, v.x) - atan2f(this->y, this->x);
	return angle < 0 ? angle += 2 * MathUtils::PI : angle;
}
NS_END

template<>
Core::JSON Core::SerializationUtils::serializeTemplated(const Maths::Vector2& value)
{
Core::JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
return json;
}

template<>
void Core::SerializationUtils::deserializeTemplated(Maths::Vector2& value, const Core::JSON& json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
}
