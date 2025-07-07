#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Metadata/EnumsManager.hpp"

NS_BEGIN(Maths)
DECLARE_ENUM(GeometricSpace,
    WORLD, "WORLD",
    SCREEN, "SCREEN"
);

class MathUtils
{
public:
	inline static const Core::f32 FLOAT_EPSILON = 0.0000001f;

	inline static const Core::f32 PI = acosf(-1.0f);			   // pi
	inline static const Core::f32 PI_2 = PI / 2.0f;			   // pi/2
	inline static const Core::f32 PI_4 = PI / 4.0f;			   // pi/4
	inline static const Core::f32 INV_PI = 1.0f / PI;			   // 1/pi
	inline static const Core::f32 INV_2_PI = 2.0f / PI;		   // 2/pi
	inline static const Core::f32 INV_2_SQRTPI = 2 / sqrtf(PI);   // 2/sqrt(pi)
	inline static const Core::f32 SQRT2 = sqrtf(2.0f);			   // sqrt(2)
	inline static const Core::f32 INV_SQRT2 = 1.0f / sqrtf(2.0f); // 1/sqrt(2)
	inline static const Core::f32 E = 2.71828182845904523536;	   // e
	inline static const Core::f32 LOG2E = log2(E);				   // log_2(e)
	inline static const Core::f32 LOG10E = log10(E);			   // log_10(e)
	inline static const Core::f32 LN2 = log1p(2.0f);			   // log_e(2)
	inline static const Core::f32 LN10 = log1p(10.0f);			   // log_e(10)

	inline static const Core::f32 PI_180 = PI / 180.0f;
	inline static const Core::f32 INV_PI_180 = 180.0f / PI;

    static Core::f32 rad(Core::f32 deg);
    static Core::f32 deg(Core::f32 rad);
    static bool eqf(Core::f32 a, Core::f32 b, Core::f32 epsilon);
    static bool eqf(Core::f32 a, Core::f32 b);
    static Core::f32 clamp(Core::f32 n, Core::f32 lower, Core::f32 upper);
    static Core::f32 det(Core::f32 a, Core::f32 b, Core::f32 c, Core::f32 d);
};

NS_END