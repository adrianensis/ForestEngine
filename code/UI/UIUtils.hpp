#pragma once

#include "Engine/Minimal.hpp"


class UIUtils
{
public:

    static Vector2 correctAspectRatio_X(const Vector2& vector);
    static Vector2 toScreenSpace(const Vector2& vector);
};

