#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/VectorDef.hpp"

#ifdef CPP_INCLUDE
#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/MathUtils.hpp"
#endif

class Vector3;
class Vector4;

class Vector2
{
public:

	f32 x = 0;
	f32 y = 0;

	VECTOR_BASE_DEFINITION(2)

	CPP Vector2() : x(0), y(0)
	{
	}

	CPP Vector2(f32 x, f32 y) : x(x), y(y)
	{
	}

	CPP Vector2(const Vector2& other) : x(other.x), y(other.y)
	{
	}

	CPP Vector2(const Vector3& other) : x(other.x), y(other.y)
	{
	}

	CPP Vector2(const Vector4& other) : x(other.x), y(other.y)
	{
	}

	CPP Vector2& set(f32 x, f32 y)
	{
		this->x = x;
		this->y = y;
		return *this;
	}

	CPP Vector2& set(const Vector2& rhs)
	{
		this->set(rhs.x, rhs.y);
		return *this;
	}

	CPP Vector2& set(const Vector3& rhs)
	{
		this->set(rhs.x, rhs.y);
		return *this;
	}

	CPP Vector2& set(const Vector4& rhs)
	{
		this->set(rhs.x, rhs.y);
		return *this;
	}

	CPP Vector2& add(const Vector2& rhs)
	{
		// can be parallelized with SIMD auto-vectorization
		x = x + rhs.x;
		y = y + rhs.y;
		return *this;
	}

	CPP Vector2& sub(const Vector2& rhs)
	{
		x = x - rhs.x;
		y = y - rhs.y;
		return *this;
	}

	CPP Vector2& mul(const Vector2& rhs)
	{
		x = x * rhs.x;
		y = y * rhs.y;
		return *this;
	}

	CPP Vector2& div(const Vector2& rhs)
	{
		ASSERT_MSG(rhs.x != 0, "Division by zero.");
		ASSERT_MSG(rhs.y != 0, "Division by zero.");
		x = x / rhs.x;
		y = y / rhs.y;
		return *this;
	}

	CPP Vector2& add(f32 rhs)
	{
		x = x + rhs;
		y = y + rhs;
		return *this;
	}

	CPP Vector2& sub(f32 rhs)
	{
		x = x - rhs;
		y = y - rhs;
		return *this;
	}

	CPP Vector2& mul(f32 rhs)
	{
		x = x * rhs;
		y = y * rhs;
		return *this;
	}

	CPP Vector2& div(f32 rhs)
	{
		ASSERT_MSG(rhs != 0, "Division by zero.");
		x = x / rhs;
		y = y / rhs;
		return *this;
	}

	CPP f32 dot(const Vector2& v) const
	{
		return this->x * v.x + this->y * v.y;
	}

	CPP f32 sqrlen() const
	{
		return this->dot(*this);
	}

	CPP f32 sqrdst(const Vector2& v) const
	{
		Vector2 sub = Vector2(v) - (*this);
		return sub.dot(sub);
	}

	CPP f32 len() const
	{
		return sqrtf(this->sqrlen());
	}

	CPP f32 max() const
	{
		return std::max(x, y);
	}

	CPP f32 min() const
	{
		return std::min(x, y);
	}

	CPP Vector2& nor()
	{
		f32 len = this->len();

		ASSERT_MSG(len > 0, "Length is zero.");
		this->div(len);

		return *this;
	}

	CPP f32 dst(const Vector2& v) const
	{
		return sqrtf(this->sqrdst(v));
	}

	CPP bool eq(const Vector2& v) const
	{
		return MathUtils::eqf(this->x, v.x) && MathUtils::eqf(this->y, v.y);
	}

	CPP bool eq(const Vector2& v, f32 e) const
	{
		return MathUtils::eqf(this->x, v.x, e) && MathUtils::eqf(this->y, v.y, e);
	}

	CPP Vector2& lerp(const Vector2& target, f32 t)
	{
		//start + percent*(end - start)
		(*this) += (Vector2(target) - (*this)) * t;
		return *this;
	}

	CPP f32 angle(const Vector2& v) const
	{
		f32 angle = atan2f(v.y, v.x) - atan2f(this->y, this->x);
		return angle < 0 ? angle += 2 * MathUtils::PI : angle;
	}

	CPP Vector2& clamp(f32 maxLength)
	{
		if (this->sqrlen() > maxLength* maxLength)
		{
			this->nor();
			this->mul(maxLength);
		}

		return *this;
	}
};

TEMPLATED_SERIALIZATION(Vector2)

#ifdef CPP_INCLUDE
template<>
JSON SerializationUtils::serializeTemplated(const Vector2& value)
{
	JSON json;
	DO_SERIALIZE("x", value.x)
	DO_SERIALIZE("y", value.y)
	return json;
}

template<>
void SerializationUtils::deserializeTemplated(Vector2& value, const JSON& json)
{
	DO_DESERIALIZE("x", value.x)
	DO_DESERIALIZE("y", value.y)
}
#endif