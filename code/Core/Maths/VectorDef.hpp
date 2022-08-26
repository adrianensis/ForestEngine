#ifndef VECTORDEF_HPP
#define VECTORDEF_HPP

#include "Core/Macros.hpp"
#include "Core/Metadata.hpp"

#define VECTOR_BASE_DEFINITION(vectorLength) \
GENERATE_METADATA_STRUCT(Vector##vectorLength)  \
\
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
    ThisClass& operator=(const ThisClass& other)\
	{\
		set(other);\
		return *this;\
	}\
    \
    ThisClass& operator+=(const ThisClass& rhs) {return this->add(rhs); }\
    ThisClass& operator-=(const ThisClass& rhs) {return this->sub(rhs); }\
    ThisClass& operator*=(const ThisClass& rhs) {return this->mul(rhs); }\
    ThisClass& operator/=(const ThisClass& rhs) {return this->div(rhs); }\
    ThisClass& operator+=(f32 rhs) {return this->add(rhs); }\
    ThisClass& operator-=(f32 rhs) {return this->sub(rhs); }\
    ThisClass& operator*=(f32 rhs) {return this->mul(rhs); }\
    ThisClass& operator/=(f32 rhs) {return this->div(rhs); }\
    bool operator==(const ThisClass& rhs) const {return this->eq(rhs); }\
    bool operator!=(const ThisClass& rhs) const {return !((*this) == rhs); }\
    ThisClass operator+(const ThisClass& rhs) const {return ThisClass(*this) += rhs; }\
    ThisClass operator-(const ThisClass& rhs) const {return ThisClass(*this) -= rhs; }\
    ThisClass operator*(const ThisClass& rhs) const {return ThisClass(*this) *= rhs; }\
    ThisClass operator/(const ThisClass& rhs) const {return ThisClass(*this) /= rhs; }\
    ThisClass operator+(f32 rhs) const {return ThisClass(*this) += rhs; }\
    ThisClass operator-(f32 rhs) const {return ThisClass(*this) -= rhs; }\
    ThisClass operator-() const{ return ThisClass(*this) *= -1; }\
    ThisClass operator*(f32 rhs) const {return ThisClass(*this) *= rhs; }\
    ThisClass operator/(f32 rhs) const {return ThisClass(*this) /= rhs; }\
    f32& operator[](const size_t index) { return get(index); }\
    f32 operator[](const size_t index) const { return get(index); }\
    ; 




#endif