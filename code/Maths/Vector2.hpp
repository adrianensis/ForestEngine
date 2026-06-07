#pragma once

#include "Maths/VectorDef.hpp"

namespace Maths
{
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
VECTOR_STATIC_DEFINITION(2)

};