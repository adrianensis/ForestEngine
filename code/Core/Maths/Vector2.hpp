#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

class Vector3;
class Vector4;

class Vector2
{
public:

	f32 x = 0;
	f32 y = 0;

	VECTOR_BASE_DEFINITION(2)

    Vector2();
    Vector2(f32 x, f32 y);
    Vector2(CR(Vector2) other);
    Vector2(CR(Vector3) other);
    Vector2(CR(Vector4) other);

    R(Vector2) set(f32 x, f32 y);
    R(Vector2) set(CR(Vector2) rhs);
    R(Vector2) set(CR(Vector3) rhs);
    R(Vector2) set(CR(Vector4) rhs);
    R(Vector2) add(CR(Vector2) rhs);
    R(Vector2) sub(CR(Vector2) rhs);
    R(Vector2) mul(CR(Vector2) rhs);
    R(Vector2) div(CR(Vector2) rhs);
    R(Vector2) add(f32 rhs);
    R(Vector2) sub(f32 rhs);
    R(Vector2) mul(f32 rhs);
    R(Vector2) div(f32 rhs);
    f32 dot(CR(Vector2) v) const;
    f32 sqrlen() const;
    f32 sqrdst(CR(Vector2) v) const;
    f32 len() const;
    f32 max() const;
    f32 min() const;
    R(Vector2) nor();
    f32 dst(CR(Vector2) v) const;
    bool eq(CR(Vector2) v) const;
    bool eq(CR(Vector2) v, f32 e) const;
    R(Vector2) lerp(CR(Vector2) target, f32 t);
    f32 angle(CR(Vector2) v) const;
    R(Vector2) clamp(f32 maxLength);
};

TEMPLATED_SERIALIZATION(Vector2)

#endif