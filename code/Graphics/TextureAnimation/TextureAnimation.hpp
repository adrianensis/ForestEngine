#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/TextureAnimation/TextureAnimationFrame.hpp"

class TextureAnimation
{
public:
    static TextureAnimation create(u32 frameCount, bool horizontal, bool reverse, const Vector2& startPosition, f32 width, f32 height, f32 speed);

    void init();
    u32 getNumberOfFrames() const;
	
public:
	std::string mName;
	std::vector<TextureAnimationFrame> mFrames;
	f32 mSpeed = 1.0f;
};

class TextureAnimationUpdater
{
public:
    void setTextureAnimation(const TextureAnimation& textureAnimation);
    const TextureAnimationFrame& nextFrame();
    const TextureAnimationFrame& getCurrentFrame() const;
private:
	u32 mCurrentFrameNumber = 0;
	f32 mTimeAccumulator = 0.0f;
    const TextureAnimation* mTextureAnimation = nullptr;
    bool mHasFrameChanged = false;
public:
    GET(HasFrameChanged)
};