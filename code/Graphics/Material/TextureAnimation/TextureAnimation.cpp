#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"


TextureAnimation::TextureAnimation()
{
	mCurrentFrameNumber = 0;
	mSpeed = 1.0f;
	mTimeAccumulator = 0.0f;
}

TextureAnimation::~TextureAnimation()  {

}
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

const TextureAnimationFrame& TextureAnimation::nextFrame()
{

	f32 time = (1.0 / (mSpeed)) * 1000.0f; // in milliseconds !

	mTimeAccumulator += GET_SYSTEM(Time).getDeltaTimeMillis();


	if (mTimeAccumulator >= time)
	{
		mTimeAccumulator = 0.0f;
		mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mFrames.size();
	}

	return mFrames.at(mCurrentFrameNumber);
}

const TextureAnimationFrame& TextureAnimation::getCurrentFrame() const
{
	return mFrames.at(mCurrentFrameNumber);
}

IMPLEMENT_SERIALIZATION(TextureAnimation)
{
	SERIALIZE("name", mName);
	SERIALIZE("speed", mSpeed);
	SERIALIZE_LIST("frames", mFrames);
}

IMPLEMENT_DESERIALIZATION(TextureAnimation)
{
	DESERIALIZE("name", mName);
	DESERIALIZE("speed", mSpeed);
	DESERIALIZE_LIST("frames", mFrames, [](const JSON& json)
	{
		TextureAnimationFrame frame;
		frame.deserialize(json);
		return frame;
	});
}
