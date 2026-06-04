#pragma once

#include "Core/Core.hpp"
#include "GPU/Core/GPUDefinitions.h"

class GPUTextureAnimationFrame
{
public:
    void init(const Maths::Vector2& position, GPU::f32 width, GPU::f32 height);

public:
    Maths::Vector2 mPosition;
	GPU::f32 mWidth = 0.0f;
	GPU::f32 mHeight = 0.0f;
};
