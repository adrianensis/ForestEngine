#include "GPU/Image/TextureAnimation/GPUTextureAnimation.hpp"
#include "Core/Time/TimeUtils.hpp"

GPUTextureAnimation GPUTextureAnimation::create(Core::u32 frameCount, bool horizontal, bool reverse, const Maths::Vector2& startPosition, Core::f32 width, Core::f32 height, Core::f32 speed)
{
	GPUTextureAnimation TextureAnimation;
	TextureAnimation.init();
	TextureAnimation.mSpeed = speed;

	Core::i32 horizontalDir = 0;
	Core::i32 verticalDir = 0;

	if (horizontal)
		horizontalDir = 1;
	else
		verticalDir = 1;

	if (reverse)
	{
		horizontalDir *= -1;
		verticalDir *= -1;
	}

	Core::i32 start = 0;
	Core::i32 end = frameCount;
	Core::i32 delta = 1;

	if (reverse)
	{
		start = frameCount - 1;
		end = 0;
		delta = INVALID_INDEX;
	}

	for (Core::i32 i = start; i != end; i += delta)
	{
		Maths::Vector2 pos(0, 0);
		pos.add(startPosition);

		if (horizontalDir != 0)
			pos.x += i * width;

		if (verticalDir != 0)
			pos.y += i * height;

		GPUTextureAnimationFrame frame;
		frame.init(pos, width, height);
		TextureAnimation.mFrames.push_back(frame);
	}

	return TextureAnimation;
}

void GPUTextureAnimation::init()
{
	mFrames.clear();
}

Core::u32 GPUTextureAnimation::getNumberOfFrames() const
{
	return mFrames.size();
}

const GPUTextureAnimationFrame& GPUTextureAnimationUpdater::nextFrame()
{
	Core::f32 time = (1.0 / (mTextureAnimation->mSpeed)) * 1000.0f; // in milliseconds !
	mTimeAccumulator += Time::Time::getInstance().getDeltaTimeMillis();
    mHasFrameChanged = false;
	if (mTimeAccumulator >= time)
	{
        mHasFrameChanged = true;
		mTimeAccumulator = 0.0f;
		mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mTextureAnimation->mFrames.size();
	}

	return mTextureAnimation->mFrames.at(mCurrentFrameNumber);
}

const GPUTextureAnimationFrame& GPUTextureAnimationUpdater::getCurrentFrame() const
{
	return mTextureAnimation->mFrames.at(mCurrentFrameNumber);
}

void GPUTextureAnimationUpdater::setTextureAnimation(const GPUTextureAnimation& textureAnimation)
{
    mCurrentFrameNumber = 0;
	mTimeAccumulator = 0.0f;
    mTextureAnimation = &textureAnimation;
}