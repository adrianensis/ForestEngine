#pragma once

#include "GPU/Image/TextureAnimation/GPUTextureAnimationFrame.hpp"

class GPUTextureAnimation
{
public:
    static GPUTextureAnimation create(Core::u32 frameCount, bool horizontal, bool reverse, const Maths::Vector2& startPosition, Core::f32 width, Core::f32 height, Core::f32 speed);

    void init();
    Core::u32 getNumberOfFrames() const;
	
public:
	std::string mName;
	std::vector<GPUTextureAnimationFrame> mFrames;
	Core::f32 mSpeed = 1.0f;
};

class GPUTextureAnimationUpdater
{
public:
    void setTextureAnimation(const GPUTextureAnimation& textureAnimation);
    const GPUTextureAnimationFrame& nextFrame();
    const GPUTextureAnimationFrame& getCurrentFrame() const;
private:
	Core::u32 mCurrentFrameNumber = 0;
	Core::f32 mTimeAccumulator = 0.0f;
    const GPUTextureAnimation* mTextureAnimation = nullptr;
    bool mHasFrameChanged = false;
public:
    GET(HasFrameChanged)
};