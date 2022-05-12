#include "Graphics/Animation/Animation.hpp"


Animation::Animation()
{
	mCurrentFrameNumber = 0;
	mSpeed = 1.0f;
	mTimeAccumulator = 0.0f;
}

Animation::~Animation()  {

}
Animation Animation::create(u32 frameCount, bool horizontal, bool reverse, const Vector2& startPosition, f32 width, f32 height, f32 speed)
{

	Animation animation;
	animation.init();
	animation.setSpeed(speed);

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
		delta = -1;
	}

	for (i32 i = start; i != end; i += delta)
	{
		Vector2 pos(0, 0);
		pos.add(startPosition);

		if (horizontalDir != 0)
			pos.x += i * width;

		if (verticalDir != 0)
			pos.y += i * height;

		AnimationFrame frame;
		frame.init(pos, width, height);
		animation.mFrames.push_back(frame);
	}

	return animation;
}

void Animation::init()
{
	mFrames.clear();
}

u32 Animation::getNumberOfFrames() const
{
	return mFrames.size();
}

const AnimationFrame& Animation::getNextFrame()
{

	f32 time = (1.0 / (mSpeed)) * 1000.0f; // in milliseconds !

	mTimeAccumulator += Time::getInstance().getDeltaTimeMillis();


	if (mTimeAccumulator >= time)
	{
		mTimeAccumulator = 0.0f;
		mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mFrames.size();
	}

	return mFrames.at(mCurrentFrameNumber);
}

const AnimationFrame& Animation::getCurrentFrame() const
{
	return mFrames.at(mCurrentFrameNumber);
}

void Animation::serialize(JSON& json) const
{
	DO_SERIALIZE("name", mName);
	DO_SERIALIZE("speed", mSpeed);
	DO_SERIALIZE_LIST("frames", mFrames);
}

void Animation::deserialize(const JSON& json)
{
	DO_DESERIALIZE("name", mName);
	DO_DESERIALIZE("speed", mSpeed);
	DO_DESERIALIZE_LIST("frames", mFrames, [](const JSON& json)
	{
		AnimationFrame frame;
		frame.deserialize(json);
		return frame;
	});
}
