#include "Graphics/Animation/AnimationFrame.hpp"


void AnimationFrame::init(const Vector2& position, f32 width, f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}

IMPLEMENT_SERIALIZATION(AnimationFrame)
{
    SERIALIZE("position", mPosition);
    SERIALIZE("width", mWidth);
    SERIALIZE("height", mHeight);
}

IMPLEMENT_DESERIALIZATION(AnimationFrame)
{
    DESERIALIZE("position", mPosition);
    DESERIALIZE("width", mWidth);
    DESERIALIZE("height", mHeight);
}
