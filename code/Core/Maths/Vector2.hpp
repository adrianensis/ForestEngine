#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector3;
class Vector4;

class Vector2
{
public:

	VECTOR_BASE_DEFINITION(2)

    Vector2(const Vector3& other);
    Vector2(const Vector4& other);

    f32 angle(const Vector2& v) const;
};

TEMPLATED_SERIALIZATION(Vector2)
