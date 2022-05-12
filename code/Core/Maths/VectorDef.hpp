#ifndef VECTORDEF_HPP
#define VECTORDEF_HPP

#include "Core/Macros.hpp"

#define VECTOR_BASE_DEFINITION(vectorLength) \
private:\
    void checkBoundaries(u32 index) const\
    {\
        ASSERT_MSG(index >= 0 && index < vectorLength, "Index out of bounds.");\
    }\
    f32& get(u32 index)\
    {\
        checkBoundaries(index);\
        return *(&x + index);\
    }\
    const f32& get(u32 index) const\
    {\
        checkBoundaries(index);\
        return *(&x + index);\
    }\
public:\
    \
    Vector##vectorLength& operator=(const Vector##vectorLength& other)\
	{\
		set(other);\
		return *this;\
	}\
    \
    Vector##vectorLength& operator+=(const Vector##vectorLength& rhs) {return this->add(rhs); }\
    Vector##vectorLength& operator-=(const Vector##vectorLength& rhs) {return this->sub(rhs); }\
    Vector##vectorLength& operator*=(const Vector##vectorLength& rhs) {return this->mul(rhs); }\
    Vector##vectorLength& operator/=(const Vector##vectorLength& rhs) {return this->div(rhs); }\
    Vector##vectorLength& operator+=(f32 rhs) {return this->add(rhs); }\
    Vector##vectorLength& operator-=(f32 rhs) {return this->sub(rhs); }\
    Vector##vectorLength& operator*=(f32 rhs) {return this->mul(rhs); }\
    Vector##vectorLength& operator/=(f32 rhs) {return this->div(rhs); }\
    bool operator==(const Vector##vectorLength& rhs) const {return this->eq(rhs); }\
    bool operator!=(const Vector##vectorLength& rhs) const {return !((*this) == rhs); }\
    Vector##vectorLength operator+(const Vector##vectorLength& rhs) const {return Vector##vectorLength(*this) += rhs; }\
    Vector##vectorLength operator-(const Vector##vectorLength& rhs) const {return Vector##vectorLength(*this) -= rhs; }\
    Vector##vectorLength operator*(const Vector##vectorLength& rhs) const {return Vector##vectorLength(*this) *= rhs; }\
    Vector##vectorLength operator/(const Vector##vectorLength& rhs) const {return Vector##vectorLength(*this) /= rhs; }\
    Vector##vectorLength operator+(f32 rhs) const {return Vector##vectorLength(*this) += rhs; }\
    Vector##vectorLength operator-(f32 rhs) const {return Vector##vectorLength(*this) -= rhs; }\
    Vector##vectorLength operator-() const{ return Vector##vectorLength(*this) *= -1; }\
    Vector##vectorLength operator*(f32 rhs) const {return Vector##vectorLength(*this) *= rhs; }\
    Vector##vectorLength operator/(f32 rhs) const {return Vector##vectorLength(*this) /= rhs; }\
    f32& operator[](const size_t index) { return get(index); }\
    f32 operator[](const size_t index) const { return get(index); }\
    ; 




#endif