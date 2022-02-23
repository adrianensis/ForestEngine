#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Serialization.hpp"
#include "Core/Maths/Matrix4.hpp"

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

	Quaternion();
	Quaternion(f32 x, f32 y, f32 z, f32 w);
	Quaternion(const Vector3& v, f32 w);
	Quaternion(f32 roll, f32 pitch, f32 yaw);
	Quaternion(const Vector3& v);
	Quaternion(const Quaternion& other);

	//-------------------------------------------------------------------
	// METHODS
	//-------------------------------------------------------------------

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

	f32 len() const;
	f32 sqrlen() const;
	f32 dot(const Quaternion& q) const;
	Quaternion& nor();
	bool eq(const Quaternion& q, f32 e) const; // epsilon equal
	bool eq(const Quaternion& q) const;		   // equal
	Quaternion& conj();
	Quaternion& inv();
	f32 angle(const Quaternion& q) const;
	Quaternion& lerp(const Quaternion& target, f32 t);
	Quaternion& nlerp(const Quaternion& target, f32 t);
	Quaternion& slerp(const Quaternion& target, f32 t);
	Quaternion& squad(); // TODO: implement
	void fromEuler(f32 roll, f32 pitch, f32 yaw);
	void fromEuler(const Vector3& v);
	//Vector3 toEuler() const;
	void toMatrix(Matrix4 * outMatrix) const;
	//void fromMatrix(const Matrix4& m);

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