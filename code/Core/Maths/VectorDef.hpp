#pragma once

#include "Core/Hash.hpp"
#include "Core/Maths/MathUtils.hpp"
#include "Core/Assert/Assert.hpp"

#define VECTOR_COMPONENT_1 x
#define VECTOR_COMPONENT_2 y
#define VECTOR_COMPONENT_3 z
#define VECTOR_COMPONENT_4 w

#define VECTOR_FOR_EACH_1(macro, ...) \
    macro(VECTOR_COMPONENT_1 __VA_OPT__(,) __VA_ARGS__)
#define VECTOR_FOR_EACH_2(macro, ...) \
    VECTOR_FOR_EACH_1(macro, __VA_ARGS__) \
    macro(VECTOR_COMPONENT_2 __VA_OPT__(,) __VA_ARGS__)
#define VECTOR_FOR_EACH_3(macro, ...) \
    VECTOR_FOR_EACH_2(macro, __VA_ARGS__) \
    macro(VECTOR_COMPONENT_3 __VA_OPT__(,) __VA_ARGS__)
#define VECTOR_FOR_EACH_4(macro, ...) \
    VECTOR_FOR_EACH_3(macro, __VA_ARGS__) \
    macro(VECTOR_COMPONENT_4 __VA_OPT__(,) __VA_ARGS__)

#define VECTOR_FOR_EACH(n, macro, ...) VECTOR_FOR_EACH_##n(macro, __VA_ARGS__)

#define VECTOR_DEFINE_COMPONENTS(comp) f32 comp = 0;

#define VECTOR2_PARAMS() f32 x, f32 y
#define VECTOR3_PARAMS() VECTOR2_PARAMS(), f32 z 
#define VECTOR4_PARAMS() VECTOR3_PARAMS(), f32 w
#define VECTOR_PARAMS(n) VECTOR##n##_PARAMS()

#define VECTOR_OP_VECTOR(comp, op) this->comp = this->comp op rhs.comp;
#define VECTOR_OP_F32(comp, op) this->comp = this->comp op rhs;
#define VECTOR_SET_VECTOR(comp) this->comp = rhs.comp;
#define VECTOR_SET_F32(comp) this->comp = comp;
#define VECTOR_APPLY_FUNC(comp, func) this->comp = func(comp);

#define VECTOR2_DOT(v) (this->x * v.x) + (this->y * v.y)
#define VECTOR3_DOT(v) VECTOR2_DOT(v) + (this->z * v.z)
#define VECTOR4_DOT(v) VECTOR3_DOT(v) + (this->w * v.w)
#define VECTOR_DOT(n, v) VECTOR##n##_DOT(v)

#define VECTOR2_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (x, y)
#define VECTOR3_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (VECTOR2_MIN_MAX(minOrMaxFunc), z)
#define VECTOR4_MIN_MAX(minOrMaxFunc) std:: minOrMaxFunc (VECTOR3_MIN_MAX(minOrMaxFunc), w)
#define VECTOR_MAX(n) VECTOR##n##_MIN_MAX(max)
#define VECTOR_MIN(n) VECTOR##n##_MIN_MAX(min)

#define VECTOR2_EQ_E(v, e) MathUtils::eqf(this->x, v.x, e) and MathUtils::eqf(this->y, v.y, e)
#define VECTOR3_EQ_E(v, e) VECTOR2_EQ_E(v, e) and MathUtils::eqf(this->z, v.z, e)
#define VECTOR4_EQ_E(v, e) VECTOR3_EQ_E(v, e) and MathUtils::eqf(this->w, v.w, e)
#define VECTOR_EQ_E(n, v, e) VECTOR##n##_EQ_E(v, e)

#define VECTOR2_EQ(v) MathUtils::eqf(this->x, v.x) and MathUtils::eqf(this->y, v.y)
#define VECTOR3_EQ(v) VECTOR2_EQ(v) and MathUtils::eqf(this->z, v.z)
#define VECTOR4_EQ(v) VECTOR3_EQ(v) and MathUtils::eqf(this->w, v.w)
#define VECTOR_EQ(n, v) VECTOR##n##_EQ(v)

#define VECTOR_BASE_DEFINITION(n) \
GENERATE_METADATA_STRUCT(Vector##n)  \
\
private:\
    void checkBoundaries(u32 index) const\
    {\
        CHECK_MSG(index >= 0 and index < n, "Index out of bounds.");\
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
    VECTOR_FOR_EACH(n, VECTOR_DEFINE_COMPONENTS); \
    \
    Vector##n() {}\
    Vector##n(VECTOR_PARAMS(n)) {VECTOR_FOR_EACH(n, VECTOR_SET_F32); }\
    Vector##n(const ThisClass& rhs) {VECTOR_FOR_EACH(n, VECTOR_SET_VECTOR);}\
    ThisClass& set(VECTOR_PARAMS(n)) {VECTOR_FOR_EACH(n, VECTOR_SET_F32); return *this; }\
    ThisClass& set(const ThisClass& rhs) {VECTOR_FOR_EACH(n, VECTOR_SET_VECTOR); return *this; }\
    ThisClass& add(const ThisClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_VECTOR, +); return *this; }\
    ThisClass& sub(const ThisClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_VECTOR, -); return *this; }\
    ThisClass& mul(const ThisClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_VECTOR, *); return *this; }\
    ThisClass& div(const ThisClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_VECTOR, /); return *this; }\
    ThisClass& add(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_F32, +); return *this; }\
    ThisClass& sub(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_F32, -); return *this; }\
    ThisClass& mul(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_F32, *); return *this; }\
    ThisClass& div(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_OP_F32, /); return *this; }\
    ThisClass& abs() { VECTOR_FOR_EACH(n, VECTOR_APPLY_FUNC, std::abs); return *this; }\
    f32 dot(const ThisClass& v) const { return VECTOR_DOT(n, v); } \
    f32 sqrlen() const { return this->dot(*this); } \
    f32 sqrdst(const ThisClass& v) const { ThisClass sub = ThisClass(v) - (*this); return sub.dot(sub); } \
    f32 len() const { return sqrtf(this->sqrlen()); } \
    f32 max() const { return VECTOR_MAX(n); } \
    f32 min() const { return VECTOR_MIN(n); } \
    ThisClass& nor() { this->div(this->len()); return *this; } \
    f32 dst(const ThisClass& v) const { return sqrtf(this->sqrdst(v)); } \
    bool eq(const ThisClass& v, f32 e) const { return VECTOR_EQ_E(n,v,e); } \
    bool eq(const ThisClass& v) const { return VECTOR_EQ(n,v); } \
    ThisClass& lerp(const ThisClass& target, f32 t) { (*this) += ((ThisClass(target) - (*this)) * t); return *this; } \
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



