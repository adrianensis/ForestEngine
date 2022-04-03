#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

#ifdef CPP_INCLUDE
#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/MathUtils.hpp"
#endif

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

	CPP Vector4() : x(0), y(0), z(0), w(0)
	{
	}

	CPP Vector4(f32 x, f32 y, f32 z, f32 w) : x(x), y(y), z(z), w(w)
	{
	}

	CPP Vector4(const Vector4& other) : x(other.x), y(other.y), z(other.z), w(other.w)
	{
	}

	CPP Vector4(const Vector3& other) : x(other.x), y(other.y), z(other.z), w(0)
	{
	}

	CPP Vector4(const Vector2& other) : x(other.x), y(other.y), z(0), w(0)
	{
	}

	CPP Vector4(const Vector3& other, f32 w) : x(other.x), y(other.y), z(other.z), w(w)
	{
	}

	CPP Vector4(const Vector2& other, f32 z, f32 w) : x(other.x), y(other.y), z(z), w(w)
	{
	}

	CPP Vector4& set(f32 x, f32 y, f32 z, f32 w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
		return *this;
	}

	CPP Vector4& set(const Vector2& rhs)
	{
		this->set(rhs.x, rhs.y, 0, 0);
		return *this;
	}

	CPP Vector4& set(const Vector3& rhs)
	{
		this->set(rhs.x, rhs.y, rhs.z, 0);
		return *this;
	}

	CPP Vector4& set(const Vector4& rhs)
	{
		this->set(rhs.x, rhs.y, rhs.z, rhs.w);
		return *this;
	}

	CPP Vector4& add(const Vector4& rhs)
	{
		// can be parallelized with SIMD auto-vectorization
		x = x + rhs.x;
		y = y + rhs.y;
		z = z + rhs.z;
		w = w + rhs.w;
		return *this;
	}

	CPP Vector4& sub(const Vector4& rhs)
	{
		x = x - rhs.x;
		y = y - rhs.y;
		z = z - rhs.z;
		w = w - rhs.w;
		return *this;
	}

	CPP Vector4& mul(const Vector4& rhs)
	{
		x = x * rhs.x;
		y = y * rhs.y;
		z = z * rhs.z;
		w = w * rhs.w;
		return *this;
	}

	CPP Vector4& div(const Vector4& rhs)
	{
		ASSERT_MSG(rhs.x != 0, "Division by zero.");
		ASSERT_MSG(rhs.y != 0, "Division by zero.");
		ASSERT_MSG(rhs.z != 0, "Division by zero.");
		ASSERT_MSG(rhs.w != 0, "Division by zero.");
		x = x / rhs.x;
		y = y / rhs.y;
		z = z / rhs.z;
		w = w / rhs.w;
		return *this;
	}

	CPP Vector4& add(f32 rhs)
	{
		x = x + rhs;
		y = y + rhs;
		z = z + rhs;
		w = w + rhs;
		return *this;
	}

	CPP Vector4& sub(f32 rhs)
	{
		x = x - rhs;
		y = y - rhs;
		z = z - rhs;
		w = w - rhs;
		return *this;
	}

	CPP Vector4& mul(f32 rhs)
	{
		x = x * rhs;
		y = y * rhs;
		z = z * rhs;
		w = w * rhs;
		return *this;
	}

	CPP Vector4& div(f32 rhs)
	{
		ASSERT_MSG(rhs != 0, "Division by zero.");
		x = x / rhs;
		y = y / rhs;
		z = z / rhs;
		w = w / rhs;
		return *this;
	}

	CPP f32 dot(const Vector4& v) const
	{
		// SIMD-optimized
		f32 xx = x * v.x;
		f32 yy = y * v.y;
		f32 zz = z * v.z;
		f32 ww = w * v.w;

		f32 dot = xx;
		dot = dot + yy;
		dot = dot + zz;
		dot = dot + ww;

		return dot;
	}

	CPP f32 sqrlen() const
	{
		return this->dot(*this);
	}

	CPP f32 sqrdst(const Vector4& v) const
	{
		Vector4 sub = Vector4(v) - (*this);
		return sub.dot(sub);
	}

	CPP f32 len() const
	{
		return sqrtf(this->sqrlen());
	}

	CPP f32 max() const
	{
		return std::max(x, std::max(y, std::max(z, w)));
	}

	CPP f32 min() const
	{
		return std::min(x, std::min(y, std::min(z, w)));
	}

	CPP Vector4& nor()
	{
		f32 len = this->len();

		ASSERT_MSG(len > 0, "Length is zero.");
		this->div(len);

		return *this;
	}

	CPP f32 dst(const Vector4& v) const
	{
		return sqrtf(this->sqrdst(v));
	}

	CPP bool eq(const Vector4& v, f32 e) const
	{
		return MathUtils::eqf(this->x, v.x, e) && MathUtils::eqf(this->y, v.y, e) && MathUtils::eqf(this->z, v.z, e) && MathUtils::eqf(this->w, v.w, e);
	}

	CPP bool eq(const Vector4& v) const
	{
		return MathUtils::eqf(this->x, v.x) && MathUtils::eqf(this->y, v.y) && MathUtils::eqf(this->z, v.z) && MathUtils::eqf(this->w, v.w);
	}

	CPP Vector4& lerp(const Vector4& target, f32 t)
	{
		//start + percent*(end - start)
		(*this) += (Vector4(target) - (*this)) * t;
		return *this;
	}

	CPP Vector4& clamp(f32 maxLength)
	{
		if (this->sqrlen() > maxLength* maxLength)
		{
			this->nor();
			this->mul(maxLength);
		}

		return *this;
	}

};

TEMPLATED_SERIALIZATION(Vector4)

#ifdef CPP_INCLUDE
template<>
JSON SerializationUtils::serializeTemplated(const Vector4& value)
{
	JSON json;
	DO_SERIALIZE("x", value.x)
	DO_SERIALIZE("y", value.y)
	DO_SERIALIZE("z", value.z)
	DO_SERIALIZE("w", value.w)
	return json;
}

template<>
void SerializationUtils::deserializeTemplated(Vector4& value, const JSON& json)
{
	DO_DESERIALIZE("x", value.x)
	DO_DESERIALIZE("y", value.y)
	DO_DESERIALIZE("z", value.z)
	DO_DESERIALIZE("w", value.w)
}
#endif