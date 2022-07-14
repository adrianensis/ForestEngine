#include "Core/Maths/Vector3.hpp"
#include "Core/Maths/Vector2.hpp"
#include "Core/Maths/Vector4.hpp"
#include "Core/Maths/MathUtils.hpp"


Vector2::Vector2() : x(0), y(0)
{
}

Vector2::Vector2(f32 x, f32 y) : x(x), y(y)
{
}

Vector2::Vector2(CR(Vector2) other) : x(other.x), y(other.y)
{
}

Vector2::Vector2(CR(Vector3) other) : x(other.x), y(other.y)
{
}

Vector2::Vector2(CR(Vector4) other) : x(other.x), y(other.y)
{
}

R(Vector2) Vector2::set(f32 x, f32 y)
{
	this->x = x;
	this->y = y;
	return *this;
}

R(Vector2) Vector2::set(CR(Vector2) rhs)
{
	this->set(rhs.x, rhs.y);
	return *this;
}

R(Vector2) Vector2::set(CR(Vector3) rhs)
{
	this->set(rhs.x, rhs.y);
	return *this;
}

R(Vector2) Vector2::set(CR(Vector4) rhs)
{
	this->set(rhs.x, rhs.y);
	return *this;
}

R(Vector2) Vector2::add(CR(Vector2) rhs)
{
	x = x + rhs.x;
	y = y + rhs.y;
	return *this;
}

R(Vector2) Vector2::sub(CR(Vector2) rhs)
{
	x = x - rhs.x;
	y = y - rhs.y;
	return *this;
}

R(Vector2) Vector2::mul(CR(Vector2) rhs)
{
	x = x * rhs.x;
	y = y * rhs.y;
	return *this;
}

R(Vector2) Vector2::div(CR(Vector2) rhs)
{
	ASSERT_MSG(rhs.x != 0, "Division by zero.");
	ASSERT_MSG(rhs.y != 0, "Division by zero.");
	x = x / rhs.x;
	y = y / rhs.y;
	return *this;
}

R(Vector2) Vector2::add(f32 rhs)
{
	x = x + rhs;
	y = y + rhs;
	return *this;
}

R(Vector2) Vector2::sub(f32 rhs)
{
	x = x - rhs;
	y = y - rhs;
	return *this;
}

R(Vector2) Vector2::mul(f32 rhs)
{
	x = x * rhs;
	y = y * rhs;
	return *this;
}

R(Vector2) Vector2::div(f32 rhs)
{
	ASSERT_MSG(rhs != 0, "Division by zero.");
	x = x / rhs;
	y = y / rhs;
	return *this;
}

f32 Vector2::dot(CR(Vector2) v) const
{
	return this->x * v.x + this->y * v.y;
}

f32 Vector2::sqrlen() const
{
	return this->dot(*this);
}

f32 Vector2::sqrdst(CR(Vector2) v) const
{
	Vector2 sub = Vector2(v) - (*this);
	return sub.dot(sub);
}

f32 Vector2::len() const
{
	return sqrtf(this->sqrlen());
}

f32 Vector2::max() const
{
	return std::max(x, y);
}

f32 Vector2::min() const
{
	return std::min(x, y);
}

R(Vector2) Vector2::nor()
{
	f32 len = this->len();

	ASSERT_MSG(len > 0, "Length is zero.");
	this->div(len);

	return *this;
}

f32 Vector2::dst(CR(Vector2) v) const
{
	return sqrtf(this->sqrdst(v));
}

bool Vector2::eq(CR(Vector2) v) const
{
	return MathUtils::eqf(this->x, v.x) && MathUtils::eqf(this->y, v.y);
}

bool Vector2::eq(CR(Vector2) v, f32 e) const
{
	return MathUtils::eqf(this->x, v.x, e) && MathUtils::eqf(this->y, v.y, e);
}

R(Vector2) Vector2::lerp(CR(Vector2) target, f32 t)
{
	(*this) += (Vector2(target) - (*this)) * t;
	return *this;
}

f32 Vector2::angle(CR(Vector2) v) const
{
	f32 angle = atan2f(v.y, v.x) - atan2f(this->y, this->x);
	return angle < 0 ? angle += 2 * MathUtils::PI : angle;
}

R(Vector2) Vector2::clamp(f32 maxLength)
{
	if (this->sqrlen() > maxLength* maxLength)
	{
		this->nor();
		this->mul(maxLength);
	}

	return *this;
}
template<>
JSON SerializationUtils::serializeTemplated(CR(Vector2) value)
{
JSON json;
SERIALIZE("x", value.x)
SERIALIZE("y", value.y)
return json;
}

template<>
void SerializationUtils::deserializeTemplated(R(Vector2) value, CR(JSON) json)
{
DESERIALIZE("x", value.x)
DESERIALIZE("y", value.y)
}

