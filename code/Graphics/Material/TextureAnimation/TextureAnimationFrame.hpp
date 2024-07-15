#pragma once

#include "Core/Minimal.hpp"


class TextureAnimationFrame
{
public:
    void init(const Vector2& position, f32 width, f32 height);

public:
    Vector2 mPosition;
	f32 mWidth = 0.0f;
	f32 mHeight = 0.0f;
};
