#pragma once

#include "Engine/Minimal.hpp"


class UIUtils
{
public:

    static Vector3 correctAspectRatio_X(const Vector3& vector);
    static Vector3 toScreenSpace(const Vector3& vector);
};

