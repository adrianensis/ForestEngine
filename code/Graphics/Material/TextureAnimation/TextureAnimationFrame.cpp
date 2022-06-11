#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"


void TextureAnimationFrame::init(const Vector2& position, f32 width, f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}

IMPLEMENT_SERIALIZATION(TextureAnimationFrame)
{
    SERIALIZE("position", mPosition);
    SERIALIZE("width", mWidth);
    SERIALIZE("height", mHeight);
}

IMPLEMENT_DESERIALIZATION(TextureAnimationFrame)
{
    DESERIALIZE("position", mPosition);
    DESERIALIZE("width", mWidth);
    DESERIALIZE("height", mHeight);
}
