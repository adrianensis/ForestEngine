#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include "Core/BasicTypes.hpp"

class MathUtils
{
public:
	inline static f32 FLOAT_EPSILON = 0.0000001f;

	inline static f32 PI = acosf(-1.0f);			   // pi
	inline static f32 PI_2 = PI / 2.0f;			   // pi/2
	inline static f32 PI_4 = PI / 4.0f;			   // pi/4
	inline static f32 INV_PI = 1.0f / PI;			   // 1/pi
	inline static f32 INV_2_PI = 2.0f / PI;		   // 2/pi
	inline static f32 INV_2_SQRTPI = 2 / sqrtf(PI);   // 2/sqrt(pi)
	inline static f32 SQRT2 = sqrtf(2.0f);			   // sqrt(2)
	inline static f32 INV_SQRT2 = 1.0f / sqrtf(2.0f); // 1/sqrt(2)
	inline static f32 E = 2.71828182845904523536;	   // e
	inline static f32 LOG2E = log2(E);				   // log_2(e)
	inline static f32 LOG10E = log10(E);			   // log_10(e)
	inline static f32 LN2 = log1p(2.0f);			   // log_e(2)
	inline static f32 LN10 = log1p(10.0f);			   // log_e(10)

	inline static f32 PI_180 = PI / 180.0f;
	inline static f32 INV_PI_180 = 180.0f / PI;

    static f32 rad(f32 deg);
    static f32 deg(f32 rad);
    static bool eqf(f32 a, f32 b, f32 epsilon);
    static bool eqf(f32 a, f32 b);
    static f32 clamp(f32 n, f32 lower, f32 upper);
    static f32 det(f32 a, f32 b, f32 c, f32 d);
};


#endif