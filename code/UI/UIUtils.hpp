#pragma once

#include "Engine/Minimal.hpp"


class UIUtils
{
public:

    static f32 correctAspectRatio(f32 x);
    static Vector3 correctAspectRatioVectorX(const Vector3& vector);
    static Vector3 toScreenSpace(const Vector3& vector);
};

