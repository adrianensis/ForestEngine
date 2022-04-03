#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Animation/AnimationFrame.hpp"
#endif

class AnimationFrame: public ObjectBase
{
    GENERATE_METADATA(AnimationFrame)

private:
    Vector2 mPosition;
	f32 mWidth = 0.0f;
	f32 mHeight = 0.0f;

public:

    COPY(AnimationFrame)
    {
        DO_COPY(mPosition)
        DO_COPY(mWidth)
        DO_COPY(mHeight)
    }

    CPP void init(const Vector2& position, f32 width, f32 height)
    {
        mPosition = position;
        mWidth = width;
        mHeight = height;
    }

    CPP void serialize(JSON& json) const
    {
        DO_SERIALIZE("position", mPosition);
        DO_SERIALIZE("width", mWidth);
        DO_SERIALIZE("height", mHeight);
    }

    CPP void deserialize(const JSON& json)
    {
        DO_DESERIALIZE("position", mPosition);
        DO_DESERIALIZE("width", mWidth);
        DO_DESERIALIZE("height", mHeight);
    }
    
    GET(Position)
    GET(Width)
    GET(Height)
};