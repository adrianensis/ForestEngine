#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector2;
class Vector4;

class Vector3
{
public:

	f32 x = 0;
	f32 y = 0;
	f32 z = 0;
	
	VECTOR_BASE_DEFINITION(3)

    Vector3();
    Vector3(f32 x, f32 y, f32 z);
    Vector3(CR(Vector3) other);
    Vector3(CR(Vector2) other);
    Vector3(CR(Vector2) other, f32 z);
    Vector3(CR(Vector4) other);
    
    R(Vector3) set(f32 x, f32 y, f32 z);
    R(Vector3) set(CR(Vector2) rhs);
    R(Vector3) set(CR(Vector3) rhs);
    R(Vector3) set(CR(Vector4) rhs);
    R(Vector3) add(CR(Vector3) rhs);
    R(Vector3) sub(CR(Vector3) rhs);
    R(Vector3) mul(CR(Vector3) rhs);
    R(Vector3) div(CR(Vector3) rhs);
    R(Vector3) add(f32 rhs);
    R(Vector3) sub(f32 rhs);
    R(Vector3) mul(f32 rhs);
    R(Vector3) div(f32 rhs);
    f32 dot(CR(Vector3) v) const;
    f32 sqrlen() const;
    f32 sqrdst(CR(Vector3) v) const;
    f32 len() const;
    f32 max() const;
    f32 min() const;
    R(Vector3) nor();
    f32 dst(CR(Vector3) v) const;
    bool eq(CR(Vector3) v) const;
    bool eq(CR(Vector3) v, f32 e) const;
    R(Vector3) cross(CR(Vector3) v);
    R(Vector3) lerp(CR(Vector3) target, f32 t);
    R(Vector3) nlerp(CR(Vector3) target, f32 t);
    R(Vector3) slerp(CR(Vector3) target, f32 t);
    f32 angle(CR(Vector3) v) const;
    f32 angle(CR(Vector3) v, CR(Vector3) n) const;
    R(Vector3) clamp(f32 maxLength);
};

TEMPLATED_SERIALIZATION(Vector3)

#endif