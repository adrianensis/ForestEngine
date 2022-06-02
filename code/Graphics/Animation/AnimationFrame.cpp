#include "Graphics/Animation/AnimationFrame.hpp"


void AnimationFrame::init(const Vector2& position, f32 width, f32 height)
{
    mPosition = position;
    mWidth = width;
    mHeight = height;
}

void AnimationFrame::serialize(JSON& json) const
{
    SERIALIZE("position", mPosition);
    SERIALIZE("width", mWidth);
    SERIALIZE("height", mHeight);
}

void AnimationFrame::deserialize(const JSON& json)
{
    DESERIALIZE("position", mPosition);
    DESERIALIZE("width", mWidth);
    DESERIALIZE("height", mHeight);
}
