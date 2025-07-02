#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"

NS_BEGIN(Maths)
Vector4::Vector4(const Vector3& other) : x(other.x), y(other.y), z(other.z), w(0)
{
}

Vector4::Vector4(const Vector2& other) : x(other.x), y(other.y), z(0), w(0)
{
}

Vector4::Vector4(const Vector3& other, Core::f32 w) : x(other.x), y(other.y), z(other.z), w(w)
{
}

Vector4::Vector4(const Vector2& other, Core::f32 z, Core::f32 w) : x(other.x), y(other.y), z(z), w(w)
{
}

NS_END
template<>
Core::JSON SerializationUtils::serializeTemplated(const Maths::Vector4& value)
{
Core::JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
SERIALIZE("z", value.z)
SERIALIZE("w", value.w)
return json;
}

template<>
void SerializationUtils::deserializeTemplated(Maths::Vector4& value, const Core::JSON& json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
DESERIALIZE("z", value.z)
DESERIALIZE("w", value.w)
}
