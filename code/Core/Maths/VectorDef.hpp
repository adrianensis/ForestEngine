#pragma once

#include "Core/Hash.hpp"
#include "Core/Maths/MathUtils.hpp"

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

#define VECTOR_DEFINE_COMPONENT(comp) f32 comp = 0;

#define VECTOR2_PARAMS_DECL() f32 x, f32 y
#define VECTOR3_PARAMS_DECL() VECTOR2_PARAMS_DECL(), f32 z 
#define VECTOR4_PARAMS_DECL() VECTOR3_PARAMS_DECL(), f32 w
#define VECTOR_PARAMS_DECL(n) VECTOR##n##_PARAMS_DECL()

#define VECTOR2_PARAMS(p) p, p
#define VECTOR3_PARAMS(p) VECTOR2_PARAMS(p), p 
#define VECTOR4_PARAMS(p) VECTOR3_PARAMS(p), p
#define VECTOR_PARAMS(n, p) VECTOR##n##_PARAMS(p)

#define VECTOR_SET_OP_VECTOR(comp, op) this->comp = this->comp op rhs.comp;
#define VECTOR_SET_OP_F32(comp, op) this->comp = this->comp op rhs;
#define VECTOR_SET_VECTOR_PARAM(comp) this->comp = rhs.comp;
#define VECTOR_SET_F32_PARAM(comp) this->comp = comp;
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

#define VECTOR2_EQ_E(v, e) MathUtils::eqf(this->x, v.x, e) && MathUtils::eqf(this->y, v.y, e)
#define VECTOR3_EQ_E(v, e) VECTOR2_EQ_E(v, e) && MathUtils::eqf(this->z, v.z, e)
#define VECTOR4_EQ_E(v, e) VECTOR3_EQ_E(v, e) && MathUtils::eqf(this->w, v.w, e)
#define VECTOR_EQ_E(n, v, e) VECTOR##n##_EQ_E(v, e)

#define VECTOR2_EQ(v) MathUtils::eqf(this->x, v.x) && MathUtils::eqf(this->y, v.y)
#define VECTOR3_EQ(v) VECTOR2_EQ(v) && MathUtils::eqf(this->z, v.z)
#define VECTOR4_EQ(v) VECTOR3_EQ(v) && MathUtils::eqf(this->w, v.w)
#define VECTOR_EQ(n, v) VECTOR##n##_EQ(v)

#define VECTOR_STATIC_DEFINITION(n) \
inline const Vector##n Vector##n::smZero = Vector##n(VECTOR_PARAMS(n, 0)) ; \
inline const Vector##n Vector##n::smOne = Vector##n(VECTOR_PARAMS(n, 1)) ; \

#define VECTOR_BASE_DEFINITION(n) \
private:\
    void checkBoundaries(u32 index) const { CHECK_MSG(index >= 0 && index < n, "Index out of bounds."); } \
    using ThisVectorClass = Vector##n;\
public:\
    VECTOR_FOR_EACH(n, VECTOR_DEFINE_COMPONENT); \
    \
    static const ThisVectorClass smZero; \
    static const ThisVectorClass smOne; \
    \
    Vector##n() {}\
    Vector##n(VECTOR_PARAMS_DECL(n)) {VECTOR_FOR_EACH(n, VECTOR_SET_F32_PARAM); }\
    Vector##n(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_VECTOR_PARAM); }\
    ThisVectorClass& set(VECTOR_PARAMS_DECL(n)) { VECTOR_FOR_EACH(n, VECTOR_SET_F32_PARAM); return *this; }\
    ThisVectorClass& set(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_VECTOR_PARAM); return *this; }\
    ThisVectorClass& add(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_VECTOR, +); return *this; }\
    ThisVectorClass& sub(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_VECTOR, -); return *this; }\
    ThisVectorClass& mul(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_VECTOR, *); return *this; }\
    ThisVectorClass& div(const ThisVectorClass& rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_VECTOR, /); return *this; }\
    ThisVectorClass& add(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_F32, +); return *this; }\
    ThisVectorClass& sub(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_F32, -); return *this; }\
    ThisVectorClass& mul(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_F32, *); return *this; }\
    ThisVectorClass& div(f32 rhs) { VECTOR_FOR_EACH(n, VECTOR_SET_OP_F32, /); return *this; }\
    ThisVectorClass& abs() { VECTOR_FOR_EACH(n, VECTOR_APPLY_FUNC, std::abs); return *this; }\
    f32 dot(const ThisVectorClass& v) const { return VECTOR_DOT(n, v); } \
    f32 sqrlen() const { return this->dot(*this); } \
    f32 sqrdst(const ThisVectorClass& v) const { ThisVectorClass sub = ThisVectorClass(v) - (*this); return sub.dot(sub); } \
    f32 len() const { return sqrtf(this->sqrlen()); } \
    f32 max() const { return VECTOR_MAX(n); } \
    f32 min() const { return VECTOR_MIN(n); } \
    ThisVectorClass& nor() { this->div(this->len()); return *this; } \
    f32 dst(const ThisVectorClass& v) const { return sqrtf(this->sqrdst(v)); } \
    bool eq(const ThisVectorClass& v, f32 e) const { return VECTOR_EQ_E(n,v,e); } \
    bool eq(const ThisVectorClass& v) const { return VECTOR_EQ(n,v); } \
    ThisVectorClass& lerp(const ThisVectorClass& target, f32 t) { (*this) += ((ThisVectorClass(target) - (*this)) * t); return *this; } \
    ThisVectorClass& clamp(f32 maxLength)\
    {\
        if (this->sqrlen() > (maxLength * maxLength))\
        {\
            this->nor();\
            this->mul(maxLength);\
        }\
        \
        return *this;\
    }\
    ThisVectorClass& operator+=(const ThisVectorClass& rhs) {return add(rhs); }\
    ThisVectorClass& operator-=(const ThisVectorClass& rhs) {return sub(rhs); }\
    ThisVectorClass& operator*=(const ThisVectorClass& rhs) {return mul(rhs); }\
    ThisVectorClass& operator/=(const ThisVectorClass& rhs) {return div(rhs); }\
    ThisVectorClass& operator+=(f32 rhs) {return add(rhs); }\
    ThisVectorClass& operator-=(f32 rhs) {return sub(rhs); }\
    ThisVectorClass& operator*=(f32 rhs) {return mul(rhs); }\
    ThisVectorClass& operator/=(f32 rhs) {return div(rhs); }\
    bool operator==(const ThisVectorClass& rhs) const {return eq(rhs); }\
    bool operator!=(const ThisVectorClass& rhs) const {return !((*this) == rhs); }\
    ThisVectorClass operator+(const ThisVectorClass& rhs) const {return ThisVectorClass(*this) += rhs; }\
    ThisVectorClass operator-(const ThisVectorClass& rhs) const {return ThisVectorClass(*this) -= rhs; }\
    ThisVectorClass operator*(const ThisVectorClass& rhs) const {return ThisVectorClass(*this) *= rhs; }\
    ThisVectorClass operator/(const ThisVectorClass& rhs) const {return ThisVectorClass(*this) /= rhs; }\
    ThisVectorClass operator+(f32 rhs) const {return ThisVectorClass(*this) += rhs; }\
    ThisVectorClass operator-(f32 rhs) const {return ThisVectorClass(*this) -= rhs; }\
    ThisVectorClass operator-() const{ return ThisVectorClass(*this) *= -1; }\
    ThisVectorClass operator*(f32 rhs) const {return ThisVectorClass(*this) *= rhs; }\
    ThisVectorClass operator/(f32 rhs) const {return ThisVectorClass(*this) /= rhs; }\
    f32& operator[](const size_t index) { checkBoundaries(index); return *(&x + index); }\
    f32 operator[](const size_t index) const { checkBoundaries(index); return *(&x + index); }\
    ; 
    