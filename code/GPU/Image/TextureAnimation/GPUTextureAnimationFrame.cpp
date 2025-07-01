#include "GPU/Image/TextureAnimation/GPUTextureAnimationFrame.hpp"

void GPUTextureAnimationFrame::init(const Vector2& position, Core::f32 width, Core::f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}