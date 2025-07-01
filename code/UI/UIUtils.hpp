#pragma once

#include "Core/Minimal.hpp"


class UIUtils
{
public:

    static Core::f32 correctAspectRatio(Core::f32 x);
    static Vector3 correctAspectRatioVectorX(const Vector3& vector);
    static Vector3 toScreenSpace(const Vector3& vector);
};

