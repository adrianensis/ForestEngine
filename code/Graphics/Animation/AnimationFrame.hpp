#ifndef ANIMATIONFRAME_HPP
#define ANIMATIONFRAME_HPP

#include "Core/Module.hpp"


class AnimationFrame: public ObjectBase
{
    GENERATE_METADATA(AnimationFrame)
    DECLARE_SERIALIZATION()

public:

    COPY(AnimationFrame)
    {
        DO_COPY(mPosition)
        DO_COPY(mWidth)
        DO_COPY(mHeight)
    }

    void init(const Vector2& position, f32 width, f32 height);

private:
    Vector2 mPosition;
	f32 mWidth = 0.0f;
	f32 mHeight = 0.0f;
public:
    GET(Position)
    GET(Width)
    GET(Height)
};

#endif