#pragma once

#include "Core/Minimal.hpp"


class UIUtils
{
public:

    static Core::f32 correctAspectRatio(Core::f32 x);
    static Maths::Vector3 correctAspectRatioVectorX(const Maths::Vector3& vector);
    static Maths::Vector3 toScreenSpace(const Maths::Vector3& vector);
};

