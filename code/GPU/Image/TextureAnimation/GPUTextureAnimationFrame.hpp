#pragma once

#include "Engine/Core.hpp"

class GPUTextureAnimationFrame
{
public:
    void init(const Maths::Vector2& position, Core::f32 width, Core::f32 height);

public:
    Maths::Vector2 mPosition;
	Core::f32 mWidth = 0.0f;
	Core::f32 mHeight = 0.0f;
};
