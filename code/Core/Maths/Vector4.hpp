#pragma once

#include "Core/Std.hpp"
#include "Core/Maths/VectorDef.hpp"

NS_BEGIN(Maths)
class Vector2;
class Vector3;

class Vector4
{
public:

	VECTOR_BASE_DEFINITION(4)

    Vector4(const Vector3& other);
    Vector4(const Vector2& other);
    Vector4(const Vector3& other, Core::f32 w);
    Vector4(const Vector2& other, Core::f32 z, Core::f32 w);
};
VECTOR_STATIC_DEFINITION(4)
NS_END
TEMPLATED_SERIALIZATION(Maths::Vector4)