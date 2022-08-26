#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/Matrix4.hpp"

class Quaternion
{
	GENERATE_METADATA_STRUCT(Quaternion);
	
public:

	Vector3 v = Vector3(0,0,0);
	f32 w = 0;

	Quaternion();
	Quaternion(f32 x, f32 y, f32 z, f32 w);
	Quaternion(const Vector3& v, f32 w);
	Quaternion(f32 roll, f32 pitch, f32 yaw);
	Quaternion(const Vector3& v);
	Quaternion(const Quaternion& other);

    Quaternion& set(f32 x, f32 y, f32 z, f32 w);
    Quaternion& set(const Vector3& v, f32 w);
    Quaternion& set(const Quaternion& rhs);
    Quaternion& add(const Quaternion& rhs);
    Quaternion& sub(const Quaternion& rhs);
    Quaternion& mul(const Quaternion& rhs);
    Quaternion& div(const Quaternion& rhs);
    Quaternion& add(f32 rhs);
    Quaternion& sub(f32 rhs);
    Quaternion& mul(f32 rhs);
    Quaternion& div(f32 rhs);
    f32 dot(const Quaternion& q) const;
    f32 sqrlen() const;
    f32 len() const;
    Quaternion& nor();
    bool eq(const Quaternion& q, f32 e) const;
    bool eq(const Quaternion& q) const;
    Quaternion& conj();
    Quaternion& inv();
    f32 angle(const Quaternion& q) const;
    Quaternion& lerp(const Quaternion& target, f32 t);
    Quaternion& nlerp(const Quaternion& target, f32 t);
    Quaternion& slerp(const Quaternion& target, f32 t);
    void fromEuler(f32 roll, f32 pitch, f32 yaw);
    void fromEuler(const Vector3& v);

	Vector3 toEuler() const
	{

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

		f32 t = m.get(0, 0) + m.get(1, 1) + m.get(2, 2);

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

    void toMatrix(Matrix4 *outMatrix) const;


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

	f32& operator[](size_t index)
	{
		ASSERT_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}

	f32 operator[](size_t index) const
	{
		ASSERT_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}
};

TEMPLATED_SERIALIZATION(Quaternion)



#endif