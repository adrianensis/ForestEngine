#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector2;
class Vector3;

class Vector4
{
public:

	f32 x = 0;
	f32 y = 0;
	f32 z = 0;
	f32 w = 0;

	VECTOR_BASE_DEFINITION(4)

    Vector4();
    Vector4(f32 x, f32 y, f32 z, f32 w);
    Vector4(CR(Vector4) other);
    Vector4(CR(Vector3) other);
    Vector4(CR(Vector2) other);
    Vector4(CR(Vector3) other, f32 w);
    Vector4(CR(Vector2) other, f32 z, f32 w);

    R(Vector4) set(f32 x, f32 y, f32 z, f32 w);
    R(Vector4) set(CR(Vector2) rhs);
    R(Vector4) set(CR(Vector3) rhs);
    R(Vector4) set(CR(Vector4) rhs);
    R(Vector4) add(CR(Vector4) rhs);
    R(Vector4) sub(CR(Vector4) rhs);
    R(Vector4) mul(CR(Vector4) rhs);
    R(Vector4) div(CR(Vector4) rhs);
    R(Vector4) add(f32 rhs);
    R(Vector4) sub(f32 rhs);
    R(Vector4) mul(f32 rhs);
    R(Vector4) div(f32 rhs);
    f32 dot(CR(Vector4) v) const;
    f32 sqrlen() const;
    f32 sqrdst(CR(Vector4) v) const;
    f32 len() const;
    f32 max() const;
    f32 min() const;
    R(Vector4) nor();
    f32 dst(CR(Vector4) v) const;
    bool eq(CR(Vector4) v, f32 e) const;
    bool eq(CR(Vector4) v) const;
    R(Vector4) lerp(CR(Vector4) target, f32 t);
    R(Vector4) clamp(f32 maxLength);

};

TEMPLATED_SERIALIZATION(Vector4)

#endif