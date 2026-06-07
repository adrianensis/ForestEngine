#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"

namespace Maths
{
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

};