#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Maths/VectorDef.hpp"
#include "Core/Serialization/Serialization.hpp"

NS_BEGIN(Maths)
class Vector2;
class Vector4;

class Vector3
{
public:
	
	VECTOR_BASE_DEFINITION(3)

    static const Vector3 smRight;
	static const Vector3 smUp;
	static const Vector3 smForward;

    Vector3(const Vector2& other);
    Vector3(const Vector2& other, Core::f32 z);
    Vector3(const Vector4& other);
    
    Vector3& cross(const Vector3& v);
    Vector3& nlerp(const Vector3& target, Core::f32 t);
    Vector3& slerp(const Vector3& target, Core::f32 t);
    Core::f32 angle(const Vector3& v) const;
    Core::f32 angle(const Vector3& v, const Vector3& n) const;
};
VECTOR_STATIC_DEFINITION(3)
inline const Vector3 Vector3::smRight = Vector3(1,0,0);
inline const Vector3 Vector3::smUp = Vector3(0,1,0);
inline const Vector3 Vector3::smForward = Vector3(0,0,1);

NS_END
TEMPLATED_SERIALIZATION(Maths::Vector3)