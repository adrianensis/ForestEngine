#include "GPU/Texture/TextureAnimation/GPUTextureAnimationFrame.hpp"

void GPUTextureAnimationFrame::init(const Maths::Vector2& position, GPU::f32 width, GPU::f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}