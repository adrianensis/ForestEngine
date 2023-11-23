#pragma once

#include "Core/Std.hpp"
#include "Core/Metadata.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector2;
class Vector4;

class Vector3
{
public:
	
	VECTOR_BASE_DEFINITION(3)

    Vector3(const Vector2& other);
    Vector3(const Vector2& other, f32 z);
    Vector3(const Vector4& other);
    
    Vector3& cross(const Vector3& v);
    Vector3& nlerp(const Vector3& target, f32 t);
    Vector3& slerp(const Vector3& target, f32 t);
    f32 angle(const Vector3& v) const;
    f32 angle(const Vector3& v, const Vector3& n) const;
};

TEMPLATED_SERIALIZATION(Vector3)
