#pragma once

#include "Core/StdCore.hpp"
#include "Core/Maths/Matrix4.hpp"
#include "Core/Serialization/Serialization.hpp"

NS_BEGIN(Maths)
class Quaternion
{
public:

	Vector3 v = Vector3::smZero;
	Core::f32 w = 0;

	Quaternion();
	Quaternion(Core::f32 x, Core::f32 y, Core::f32 z, Core::f32 w);
	Quaternion(const Vector3& v, Core::f32 w);
	Quaternion(Core::f32 roll, Core::f32 pitch, Core::f32 yaw);
	Quaternion(const Vector3& v);
	Quaternion(const Vector4& v);
	Quaternion(const Quaternion& other);

    Quaternion& set(Core::f32 x, Core::f32 y, Core::f32 z, Core::f32 w);
    Quaternion& set(const Vector3& v, Core::f32 w);
    Quaternion& set(const Quaternion& rhs);
    Quaternion& add(const Quaternion& rhs);
    Quaternion& sub(const Quaternion& rhs);
    Quaternion& mul(const Quaternion& rhs);
    Quaternion& div(const Quaternion& rhs);
    Quaternion& add(Core::f32 rhs);
    Quaternion& sub(Core::f32 rhs);
    Quaternion& mul(Core::f32 rhs);
    Quaternion& div(Core::f32 rhs);
    Core::f32 dot(const Quaternion& q) const;
    Core::f32 sqrlen() const;
    Core::f32 len() const;
    Quaternion& nor();
    bool eq(const Quaternion& q, Core::f32 e) const;
    bool eq(const Quaternion& q) const;
    Quaternion& conj();
    Quaternion& inv();
    Core::f32 angle(const Quaternion& q) const;
    Quaternion& lerp(const Quaternion& target, Core::f32 t);
    Quaternion& nlerp(const Quaternion& target, Core::f32 t);
    Quaternion& slerp(const Quaternion& target, Core::f32 t);
	Vector3 toEuler() const;
    void fromEuler(Core::f32 roll, Core::f32 pitch, Core::f32 yaw);
    void fromEuler(const Vector3& v);

    void toMatrix(Matrix4& outMatrix) const;
    void fromMatrix(const Matrix4& matrix);

    Quaternion operator=(const Quaternion& other)
    {
        if (this != &other)
        {
            set(other);
        }
        return *this;
    }

	Quaternion& operator+=(const Quaternion& rhs) { return this->add(rhs); }
	Quaternion& operator-=(const Quaternion& rhs) { return this->sub(rhs); }
	Quaternion& operator*=(const Quaternion& rhs) { return this->mul(rhs); }
	Quaternion& operator/=(const Quaternion& rhs) { return this->div(rhs); }
	Quaternion& operator+=(Core::f32 rhs) { return this->add(rhs); }
	Quaternion& operator-=(Core::f32 rhs) { return this->sub(rhs); }
	Quaternion& operator*=(Core::f32 rhs) { return this->mul(rhs); }
	Quaternion& operator/=(Core::f32 rhs) { return this->div(rhs); }
	bool operator==(const Quaternion& rhs) const { return this->eq(rhs); }
	bool operator!=(const Quaternion& rhs) const { return !((*this) == rhs); }
	Quaternion operator+(const Quaternion& rhs) const { return Quaternion(*this) += rhs; }
	Quaternion operator-(const Quaternion& rhs) const { return Quaternion(*this) -= rhs; }
	Quaternion operator*(const Quaternion& rhs) const { return Quaternion(*this) *= rhs; }
	Quaternion operator/(const Quaternion& rhs) const { return Quaternion(*this) /= rhs; }
	Quaternion operator+(Core::f32 rhs) const { return Quaternion(*this) += rhs; }
	Quaternion operator-(Core::f32 rhs) const { return Quaternion(*this) -= rhs; }
	Quaternion operator*(Core::f32 rhs) const { return Quaternion(*this) *= rhs; }
	Quaternion operator/(Core::f32 rhs) const { return Quaternion(*this) /= rhs; }

	Core::f32& operator[](size_t index)
	{
		CHECK_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}

	Core::f32 operator[](size_t index) const
	{
		CHECK_MSG(index >= 0 && index < 4, "Index out of bounds.");
		if (index == 3)
			return w;
		return v[index];
	}
};

NS_END
TEMPLATED_SERIALIZATION(Maths::Quaternion)

