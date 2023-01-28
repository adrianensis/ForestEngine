#pragma once

#include "Core/Macros.hpp"
#include "Core/Metadata.hpp"
#include "Core/Maths/MathUtils.hpp"

#define VECTOR2_MEMBERS() \
    f32 x = 0; \
    f32 y = 0; 

#define VECTOR3_MEMBERS() \
    VECTOR2_MEMBERS(); \
    f32 z = 0; 

#define VECTOR4_MEMBERS() \
    VECTOR3_MEMBERS(); \
    f32 w = 0; 

#define VECTOR2_PARAMS() f32 x, f32 y
#define VECTOR3_PARAMS() VECTOR2_PARAMS(), f32 z
#define VECTOR4_PARAMS() VECTOR3_PARAMS(), f32 w

#define VECTOR2_OP_VECTOR(op) \
    this->x = this->x op rhs.x; \
    this->y = this->y op rhs.y; 

#define VECTOR3_OP_VECTOR(op) \
    VECTOR2_OP_VECTOR(op); \
    this->z = this->z op rhs.z; 

#define VECTOR4_OP_VECTOR(op) \
    VECTOR3_OP_VECTOR(op); \
    this->w = this->w op rhs.w;
    
#define VECTOR2_OP_F32(op) \
    this->x = this->x op rhs; \
    this->y = this->y op rhs; 

#define VECTOR3_OP_F32(op) \
    VECTOR2_OP_F32(op); \
    this->z = this->z op rhs; 

#define VECTOR4_OP_F32(op) \
    VECTOR3_OP_F32(op); \
    this->w = this->w op rhs; 

#define VECTOR2_SET_F32() \
    this->x = x; \
    this->y = y; 

#define VECTOR3_SET_F32() \
    VECTOR2_SET_F32(); \
    this->z = z; 

#define VECTOR4_SET_F32() \
    VECTOR3_SET_F32(); \
    this->w = w; 

#define VECTOR2_SET_VECTOR() \
    this->x = rhs.x; \
    this->y = rhs.y; 

#define VECTOR3_SET_VECTOR() \
    VECTOR2_SET_VECTOR(); \
    this->z = rhs.z; 

#define VECTOR4_SET_VECTOR() \
    VECTOR3_SET_VECTOR(); \
    this->w = rhs.w; 

#define VECTOR2_DOT(v) (this->x * v.x) + (this->y * v.y)
#define VECTOR3_DOT(v) VECTOR2_DOT(v) + (this->z * v.z)
#define VECTOR4_DOT(v) VECTOR3_DOT(v) + (this->w * v.w)

#define VECTOR2_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (x, y)
#define VECTOR3_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (VECTOR2_MIN_MAX(minOrMaxFunc), z)
#define VECTOR4_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (VECTOR3_MIN_MAX(minOrMaxFunc), w)

#define VECTOR2_EQ_E(v, e) MathUtils::eqf(this->x, v.x, e) and MathUtils::eqf(this->y, v.y, e)
#define VECTOR3_EQ_E(v, e) VECTOR2_EQ_E(v, e) and MathUtils::eqf(this->z, v.z, e)
#define VECTOR4_EQ_E(v, e) VECTOR3_EQ_E(v, e) and MathUtils::eqf(this->w, v.w, e)

#define VECTOR2_EQ(v) MathUtils::eqf(this->x, v.x) and MathUtils::eqf(this->y, v.y)
#define VECTOR3_EQ(v) VECTOR2_EQ(v) and MathUtils::eqf(this->z, v.z)
#define VECTOR4_EQ(v) VECTOR3_EQ(v) and MathUtils::eqf(this->w, v.w)

#define VECTOR_BASE_DEFINITION(vectorLength) \
GENERATE_METADATA_STRUCT(Vector##vectorLength)  \
\
private:\
    void checkBoundaries(u32 index) const\
    {\
        ASSERT_MSG(index >= 0 and index < vectorLength, "Index out of bounds.");\
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
    VECTOR##vectorLength##_MEMBERS(); \
    \
    Vector##vectorLength() {}\
    Vector##vectorLength(VECTOR##vectorLength##_PARAMS()) {VECTOR##vectorLength##_SET_F32();}\
    Vector##vectorLength(const ThisClass& rhs) {VECTOR##vectorLength##_SET_VECTOR();}\
    ThisClass& set(VECTOR##vectorLength##_PARAMS()) {VECTOR##vectorLength##_SET_F32(); return *this; }\
    ThisClass& set(const ThisClass& rhs) {VECTOR##vectorLength##_SET_VECTOR(); return *this; }\
    ThisClass& add(const ThisClass& rhs) { VECTOR##vectorLength##_OP_VECTOR(+); return *this; }\
    ThisClass& sub(const ThisClass& rhs) { VECTOR##vectorLength##_OP_VECTOR(-); return *this; }\
    ThisClass& mul(const ThisClass& rhs) { VECTOR##vectorLength##_OP_VECTOR(*); return *this; }\
    ThisClass& div(const ThisClass& rhs) { VECTOR##vectorLength##_OP_VECTOR(/); return *this; }\
    ThisClass& add(f32 rhs) { VECTOR##vectorLength##_OP_F32(+); return *this; }\
    ThisClass& sub(f32 rhs) { VECTOR##vectorLength##_OP_F32(-); return *this; }\
    ThisClass& mul(f32 rhs) { VECTOR##vectorLength##_OP_F32(*); return *this; }\
    ThisClass& div(f32 rhs) { VECTOR##vectorLength##_OP_F32(/); return *this; }\
    f32 dot(const ThisClass& v) const { return VECTOR##vectorLength##_DOT(v); } \
    f32 sqrlen() const { return this->dot(*this); } \
    f32 sqrdst(const ThisClass& v) const { ThisClass sub = ThisClass(v) - (*this); return sub.dot(sub); } \
    f32 len() const { return sqrtf(this->sqrlen()); } \
    f32 max() const { return VECTOR##vectorLength##_MIN_MAX(max); } \
    f32 min() const { return VECTOR##vectorLength##_MIN_MAX(min); } \
    ThisClass& nor() { this->div(this->len()); return *this; } \
    f32 dst(const ThisClass& v) const { return sqrtf(this->sqrdst(v)); } \
    bool eq(const ThisClass& v, f32 e) const { return VECTOR##vectorLength##_EQ_E(v,e); } \
    bool eq(const ThisClass& v) const { return VECTOR##vectorLength##_EQ(v); } \
    ThisClass& lerp(const ThisClass& target, f32 t) { (*this) += (ThisClass(target) - (*this)) * t; return *this; } \
    ThisClass& clamp(f32 maxLength)\
    {\
        if (this->sqrlen() > (maxLength * maxLength))\
        {\
            this->nor();\
            this->mul(maxLength);\
        }\
        \
        return *this;\
    }\
    ThisClass& operator+=(const ThisClass& rhs) {return add(rhs); }\
    ThisClass& operator-=(const ThisClass& rhs) {return sub(rhs); }\
    ThisClass& operator*=(const ThisClass& rhs) {return mul(rhs); }\
    ThisClass& operator/=(const ThisClass& rhs) {return div(rhs); }\
    ThisClass& operator+=(f32 rhs) {return add(rhs); }\
    ThisClass& operator-=(f32 rhs) {return sub(rhs); }\
    ThisClass& operator*=(f32 rhs) {return mul(rhs); }\
    ThisClass& operator/=(f32 rhs) {return div(rhs); }\
    bool operator==(const ThisClass& rhs) const {return eq(rhs); }\
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



