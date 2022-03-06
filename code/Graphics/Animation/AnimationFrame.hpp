#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"

class AnimationFrame: public ObjectBase
{
    GENERATE_METADATA(AnimationFrame)

private:
    Vector2 mPosition;
	f32 mWidth = 0.0f;
	f32 mHeight = 0.0f;

public:
	void init(const Vector2& position, f32 width, f32 height);

    COPY(AnimationFrame)
    {
        DO_COPY(mPosition)
        DO_COPY(mWidth)
        DO_COPY(mHeight)
    }

    void serialize(JSON& json) const override;
    void deserialize(const JSON& json) override;

    GET(Position)
    GET(Width)
    GET(Height)
};