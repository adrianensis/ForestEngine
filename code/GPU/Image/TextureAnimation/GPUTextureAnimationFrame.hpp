#pragma once

#include "Core/Minimal.hpp"

class GPUTextureAnimationFrame
{
public:
    void init(const Vector2& position, Core::f32 width, Core::f32 height);

public:
    Vector2 mPosition;
	Core::f32 mWidth = 0.0f;
	Core::f32 mHeight = 0.0f;
};
