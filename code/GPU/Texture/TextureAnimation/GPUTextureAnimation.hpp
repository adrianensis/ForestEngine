#pragma once

#include "GPU/Texture/TextureAnimation/GPUTextureAnimationFrame.hpp"

class GPUTextureAnimation
{
public:
    static GPUTextureAnimation create(GPU::u32 frameCount, bool horizontal, bool reverse, const Maths::Vector2& startPosition, GPU::f32 width, GPU::f32 height, GPU::f32 speed);

    void init();
    GPU::u32 getNumberOfFrames() const;
	
public:
	std::string mName;
	std::vector<GPUTextureAnimationFrame> mFrames;
	GPU::f32 mSpeed = 1.0f;
};

class GPUTextureAnimationUpdater
{
public:
    void setTextureAnimation(const GPUTextureAnimation& textureAnimation);
    const GPUTextureAnimationFrame& nextFrame(GPU::f32 dt);
    const GPUTextureAnimationFrame& getCurrentFrame() const;
private:
	GPU::u32 mCurrentFrameNumber = 0;
	GPU::f32 mTimeAccumulator = 0.0f;
    const GPUTextureAnimation* mTextureAnimation = nullptr;
    bool mHasFrameChanged = false;
public:
    auto getHasFrameChanged() const { return mHasFrameChanged; }
};