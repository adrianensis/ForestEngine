#include "GPU/Image/TextureAnimation/GPUTextureAnimationFrame.hpp"

void GPUTextureAnimationFrame::init(const Vector2& position, f32 width, f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}