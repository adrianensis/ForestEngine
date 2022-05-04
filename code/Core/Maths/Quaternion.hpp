#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/Matrix4.hpp"

#ifdef CPP_INCLUDE
#include "Core/Maths/Quaternion.hpp"
#include "Core/Maths/MathUtils.hpp"
#endif

class Quaternion
{
public:
	//-------------------------------------------------------------------
	// MEMBERS
	//-------------------------------------------------------------------

	Vector3 v = Vector3(0,0,0);
	f32 w = 0;

	//-------------------------------------------------------------------
	// CONSTRUCTORS/DESTRUCTOR
	//-------------------------------------------------------------------

	CPP Quaternion()
	{
	}

	CPP Quaternion(f32 x, f32 y, f32 z, f32 w) : v(x, y, z), w(w)
	{
	}

	CPP Quaternion(const Vector3& v, f32 w) : v(v), w(w)
	{
	}

	CPP Quaternion(f32 roll, f32 pitch, f32 yaw)
	{
		fromEuler(roll, pitch, yaw);
	}

	CPP Quaternion(const Vector3& v) : Quaternion(v.x, v.y, v.z)
	{
	}

	CPP Quaternion(const Quaternion& other) : v(other.v), w(other.w)
	{
	}

	//-------------------------------------------------------------------
	// METHODS
	//-------------------------------------------------------------------

	CPP Quaternion& set(f32 x, f32 y, f32 z, f32 w)
	{
		v.set(x, y, z);
		this->w = w;
		return *this;
	}

	CPP Quaternion& set(const Vector3& v, f32 w)
	{
		this->v.set(v);
		this->w = w;
		return *this;
	}

	CPP Quaternion& set(const Quaternion& rhs)
	{
		set(rhs.v, rhs.w);
		return *this;
	}

	CPP Quaternion& add(const Quaternion& rhs)
	{
		// can be parallelized with SIMD auto-vectorization
		v.add(rhs.v);
		w = w + rhs.w;
		return *this;
	}

	CPP Quaternion& sub(const Quaternion& rhs)
	{
		v.sub(rhs.v);
		w = w - rhs.w;
		return *this;
	}

	CPP Quaternion& mul(const Quaternion& rhs)
	{
		f32 w_total = (w * rhs.w) - (v.dot(rhs.v));
		v.set(rhs.v * w + v * w + Vector3(v).cross(rhs.v));
		w = w_total;
		return *this;
	}

	CPP Quaternion& div(const Quaternion& rhs)
	{
		this->mul(Quaternion(rhs).inv());
		return *this;
	}

	CPP Quaternion& add(f32 rhs)
	{
		v.add(rhs);
		w = w + rhs;
		return *this;
	}

	CPP Quaternion& sub(f32 rhs)
	{
		v.sub(rhs);
		w = w - rhs;
		return *this;
	}

	CPP Quaternion& mul(f32 rhs)
	{
		v.mul(rhs);
		w = w * rhs;
		return *this;
	}

	CPP Quaternion& div(f32 rhs)
	{
		ASSERT_MSG(rhs != 0, "Division by zero.");
		v.div(rhs);
		w = w / rhs;
		return *this;
	}

	CPP f32 dot(const Quaternion& q) const
	{
		// SIMD-optimized
		f32 xx = v.x * q.v.x;
		f32 yy = v.y * q.v.y;
		f32 zz = v.z * q.v.z;
		f32 ww = w * q.w;

		return xx + yy + zz + ww;
	}

	CPP f32 sqrlen() const
	{
		return dot(*this);
	}

	CPP f32 len() const
	{
		return sqrtf(this->sqrlen());
	}

	CPP Quaternion& nor()
	{
		f32 len = this->len();

		ASSERT_MSG(len > 0, "Length is zero.");
		this->div(len);

		return *this;
	}

	CPP bool eq(const Quaternion& q, f32 e) const
	{
		return v.eq(q.v, e) && MathUtils::eqf(this->w, q.w, e);
	}

	CPP bool eq(const Quaternion& q) const
	{
		return v.eq(q.v) && MathUtils::eqf(this->w, q.w);
	}

	CPP Quaternion& conj()
	{
		this->v.mul(-1);
		return *this;
	}

	CPP Quaternion& inv()
	{
		this->conj().div(this->sqrlen());
		return *this;
	}

	CPP f32 angle(const Quaternion& q) const
	{
		/*
		* angle is acute (positive dot product)
		* perpendicular (zero dot product)
		* or obtuse (negative dot product).
		*/
		// between 0 - 180
		return acosf(v.dot(q.v) / (v.len() * q.v.len()));
	}

	CPP Quaternion& lerp(const Quaternion& target, f32 t)
	{
		f32 tt = 1 - t;
		this->mul(tt);
		this->add(Quaternion(target).mul(tt));
		return *this;
	}

	CPP Quaternion& nlerp(const Quaternion& target, f32 t)
	{
		this->lerp(target, t).nor();
		return *this;
	}

	CPP Quaternion& slerp(const Quaternion& target, f32 t)
	{
		f32 theta = angle(target);

		f32 sinTheta = sinf(theta);

		this->mul((sinf(1 - t) * theta) / sinTheta).add(Quaternion(target).mul((sinf(t * theta)) / sinTheta));
		return *this;
	}

	CPP void fromEuler(f32 roll, f32 pitch, f32 yaw)
	{ // pitch attitude, yaw heading, or roll bank

		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		f32 roll2 = MathUtils::rad(roll) * 0.5f;   // x
		f32 pitch2 = MathUtils::rad(pitch) * 0.5f; // y
		f32 yaw2 = MathUtils::rad(yaw) * 0.5f;	   // z

		f32 cy = cos(yaw2);
		f32 sy = sin(yaw2);
		f32 cp = cos(pitch2);
		f32 sp = sin(pitch2);
		f32 cr = cos(roll2);
		f32 sr = sin(roll2);

		// original
		w = cr * cp * cy + sr * sp * sy;
		v.x = sr * cp * cy - cr * sp * sy;
		v.y = cr * sp * cy + sr * cp * sy;
		v.z = cr * cp * sy - sr * sp * cy;
	}

	CPP void fromEuler(const Vector3& v)
	{
		fromEuler(v.x, v.y, v.z);
	}

	Vector3 toEuler() const
	{
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

		Vector3 eulerVec;

		f32 t0 = 2 * (w * v.x + v.y * v.z);
        f32 t1 = 1 - 2 * (v.x * v.x + v.y * v.y);
        eulerVec.x = std::atan2(t0, t1);
 
        f32 t2 = 2 * (w * v.y - v.z * v.x);
		t2 = t2 > 1 ?  1 : t2;
		t2 = t2 < -1 ? -1 : t2;
        eulerVec.y = std::asin(t2);
         
        f32 t3 = 2 * (w * v.z + v.x * v.y);
        f32 t4 = 1 - 2 * (v.y * v.y + v.z * v.z);
        eulerVec.z = std::atan2(t3, t4);
 
        return eulerVec;
	}

	/*void fromMatrix(const Matrix4& m){
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/

		f32 t = m.get(0, 0) + m.get(1, 1) + m.get(2, 2);

		// we protect the division by s by ensuring that s>=1
		if (t >= 0){ // |w| >= .5

			f32 s = sqrtf(t + 1); // |s|>=1 ...
			w = 0.5f * s;
			s = 0.5f / s;                 // so this division isn't bad
			v.x = (m.get(1, 2) - m.get(2, 1)) * s;
			v.y = (m.get(2, 0) - m.get(0, 2)) * s;
			v.z = (m.get(0, 1) - m.get(1, 0)) * s;

		} else if ((m.get(0, 0) > m.get(1, 1)) && (m.get(0, 0) > m.get(2, 2))){
			f32 s = sqrtf(1.0f + m.get(0, 0) - m.get(1, 1) - m.get(2, 2)); // |s|>=1
			v.x = s * 0.5f; // |x| >= .5
			s = 0.5f / s;
			v.y = (m.get(0, 1) + m.get(1, 0)) * s;
			v.z = (m.get(2, 0) + m.get(0, 2)) * s;
			w = (m.get(1, 2) - m.get(2, 1)) * s;

		} else if (m.get(1, 1) > m.get(2, 2)){
			f32 s = sqrtf(1.0f + m.get(1, 1) - m.get(0, 0) - m.get(2, 2)); // |s|>=1
			v.y = s * 0.5f; // |y| >= .5
			s = 0.5f / s;
			v.x = (m.get(0, 1) + m.get(1, 0)) * s;
			v.z = (m.get(1, 2) + m.get(2, 1)) * s;
			w = (m.get(2, 0) - m.get(0, 2)) * s;

		} else {
			f32 s = sqrtf(1.0f + m.get(2, 2) - m.get(0, 0) - m.get(1, 1)); // |s|>=1
			v.z = s * 0.5f; // |z| >= .5
			s = 0.5f / s;
			v.x = (m.get(2, 0) + m.get(0, 2)) * s;
			v.y = (m.get(1, 2) + m.get(2, 1)) * s;
			w = (m.get(0, 1) - m.get(1, 0)) * s;

		}

	}*/

	CPP void toMatrix(Matrix4 *outMatrix) const
	{
		Quaternion copy((*this));
		copy.nor();

		f32 xx2 = 2 * copy.v.x * copy.v.x;
		f32 yy2 = 2 * copy.v.y * copy.v.y;
		f32 zz2 = 2 * copy.v.z * copy.v.z;

		f32 xy2 = 2 * copy.v.x * copy.v.y;
		f32 xz2 = 2 * copy.v.x * copy.v.z;
		f32 yz2 = 2 * copy.v.y * copy.v.z;

		f32 wx2 = 2 * copy.w * copy.v.x;
		f32 wy2 = 2 * copy.w * copy.v.y;
		f32 wz2 = 2 * copy.w * copy.v.z;

		outMatrix->identity();

		outMatrix->set(0, 0, 1 - (yy2 + zz2));
		outMatrix->set(0, 1, xy2 + wz2);
		outMatrix->set(0, 2, xz2 - wy2);

		outMatrix->set(1, 0, xy2 - wz2);
		outMatrix->set(1, 1, 1 - (xx2 + zz2));
		outMatrix->set(1, 2, yz2 + wx2);

		outMatrix->set(2, 0, xz2 + wy2);
		outMatrix->set(2, 1, yz2 - wx2);
		outMatrix->set(2, 2, 1 - (xx2 + yy2));
	}

	//-------------------------------------------------------------------
	// OPERATORS
	//-------------------------------------------------------------------

	/*
	* & -> it's a reference, not a full copy.
	* Quaternion& -> it's a constant reference.
	* operator=(...) const -> the method promises not to change *this. Non-member
	* functions can not have constant qualification.
	*/

	Quaternion& operator=(const Quaternion& rhs)
	{
			return this->set(rhs);
	}

	Quaternion& operator+=(const Quaternion& rhs)
	{
		// can be parallelized with SIMD auto-vectorization
		return this->add(rhs);
	}

	Quaternion& operator-=(const Quaternion& rhs)
	{
		return this->sub(rhs);
	}

	Quaternion& operator*=(const Quaternion& rhs)
	{
		return this->mul(rhs);
	}

	Quaternion& operator/=(const Quaternion& rhs)
	{
		return this->div(rhs);
	}

	Quaternion& operator+=(f32 rhs)
	{
		return this->add(rhs);
	}

	Quaternion& operator-=(f32 rhs)
	{
		return this->sub(rhs);
	}

	Quaternion& operator*=(f32 rhs)
	{
		return this->mul(rhs);
	}

	Quaternion& operator/=(f32 rhs)
	{
		return this->div(rhs);
	}

	bool operator==(const Quaternion& rhs) const
	{
		return this->eq(rhs);
	}

	bool operator!=(const Quaternion& rhs) const
	{
		return !((*this) == rhs);
	}

	Quaternion operator+(const Quaternion& rhs) const
	{
		return Quaternion(*this) += rhs;
	}

	Quaternion operator-(const Quaternion& rhs) const
	{
		return Quaternion(*this) -= rhs;
	}

	Quaternion operator*(const Quaternion& rhs) const
	{
		return Quaternion(*this) *= rhs;
	}

	Quaternion operator/(const Quaternion& rhs) const
	{
		return Quaternion(*this) /= rhs;
	}

	Quaternion operator+(f32 rhs) const
	{
		return Quaternion(*this) += rhs;
	}

	Quaternion operator-(f32 rhs) const
	{
		return Quaternion(*this) -= rhs;
	}

	Quaternion operator*(f32 rhs) const
	{
		return Quaternion(*this) *= rhs;
	}

	Quaternion operator/(f32 rhs) const
	{
		return Quaternion(*this) /= rhs;
	}

	// can be used for assignment
	f32& operator[](size_t index)
	{
		ASSERT_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}

	// read only
	f32 operator[](size_t index) const
	{
		ASSERT_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}
};

TEMPLATED_SERIALIZATION(Quaternion)

#ifdef CPP_INCLUDE
template<>
JSON SerializationUtils::serializeTemplated(const Quaternion& value)
{
	JSON json;
	DO_SERIALIZE("v", value.v)
	DO_SERIALIZE("w", value.w)
	return json;
}

template<>
void SerializationUtils::deserializeTemplated(Quaternion& value, const JSON& json)
{
	DO_DESERIALIZE("v", value.v)
	DO_DESERIALIZE("w", value.w)
}
#endif