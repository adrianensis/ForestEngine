#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"

Vector4::Vector4(const Vector3& other) : x(other.x), y(other.y), z(other.z), w(0)
{
}

Vector4::Vector4(const Vector2& other) : x(other.x), y(other.y), z(0), w(0)
{
}

Vector4::Vector4(const Vector3& other, f32 w) : x(other.x), y(other.y), z(other.z), w(w)
{
}

Vector4::Vector4(const Vector2& other, f32 z, f32 w) : x(other.x), y(other.y), z(z), w(w)
{
}

template<>
JSON SerializationUtils::serializeTemplated(const Vector4& value)
{
JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
SERIALIZE("z", value.z)
SERIALIZE("w", value.w)
return json;
}

template<>
void SerializationUtils::deserializeTemplated(Vector4& value, const JSON& json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
DESERIALIZE("z", value.z)
DESERIALIZE("w", value.w)
}

