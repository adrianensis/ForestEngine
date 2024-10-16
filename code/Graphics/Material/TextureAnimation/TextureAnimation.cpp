#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Core/Time/TimeUtils.hpp"

TextureAnimation TextureAnimation::create(u32 frameCount, bool horizontal, bool reverse, const Vector2& startPosition, f32 width, f32 height, f32 speed)
{
	TextureAnimation TextureAnimation;
	TextureAnimation.init();
	TextureAnimation.mSpeed = speed;

	i32 horizontalDir = 0;
	i32 verticalDir = 0;

	if (horizontal)
		horizontalDir = 1;
	else
		verticalDir = 1;

	if (reverse)
	{
		horizontalDir *= -1;
		verticalDir *= -1;
	}

	i32 start = 0;
	i32 end = frameCount;
	i32 delta = 1;

	if (reverse)
	{
		start = frameCount - 1;
		end = 0;
		delta = INVALID_INDEX;
	}

	for (i32 i = start; i != end; i += delta)
	{
		Vector2 pos(0, 0);
		pos.add(startPosition);

		if (horizontalDir != 0)
			pos.x += i * width;

		if (verticalDir != 0)
			pos.y += i * height;

		TextureAnimationFrame frame;
		frame.init(pos, width, height);
		TextureAnimation.mFrames.push_back(frame);
	}

	return TextureAnimation;
}

void TextureAnimation::init()
{
	mFrames.clear();
}

u32 TextureAnimation::getNumberOfFrames() const
{
	return mFrames.size();
}

const TextureAnimationFrame& TextureAnimationUpdater::nextFrame()
{
	f32 time = (1.0 / (mTextureAnimation->mSpeed)) * 1000.0f; // in milliseconds !
	mTimeAccumulator += GET_SYSTEM(Time).getDeltaTimeMillis();
    mHasFrameChanged = false;
	if (mTimeAccumulator >= time)
	{
        mHasFrameChanged = true;
		mTimeAccumulator = 0.0f;
		mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mTextureAnimation->mFrames.size();
	}

	return mTextureAnimation->mFrames.at(mCurrentFrameNumber);
}

const TextureAnimationFrame& TextureAnimationUpdater::getCurrentFrame() const
{
	return mTextureAnimation->mFrames.at(mCurrentFrameNumber);
}

void TextureAnimationUpdater::setTextureAnimation(const TextureAnimation& textureAnimation)
{
    mCurrentFrameNumber = 0;
	mTimeAccumulator = 0.0f;
    mTextureAnimation = &textureAnimation;
}