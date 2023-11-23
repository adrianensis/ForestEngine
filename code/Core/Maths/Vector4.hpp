#pragma once

#include "Core/Std.hpp"
#include "Core/Metadata.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector2;
class Vector3;

class Vector4
{
public:

	VECTOR_BASE_DEFINITION(4)

    Vector4(const Vector3& other);
    Vector4(const Vector2& other);
    Vector4(const Vector3& other, f32 w);
    Vector4(const Vector2& other, f32 z, f32 w);
};

TEMPLATED_SERIALIZATION(Vector4)
