#pragma once

#include "Core/Module.hpp"
#include "Graphics/Animation/AnimationFrame.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Animation/Animation.hpp"
#endif

class Animation: public ObjectBase
{
    GENERATE_METADATA(Animation)

private:
	std::string mName;
	std::vector<AnimationFrame> mFrames;
	u32 mCurrentFrameNumber = 0;
	f32 mTimeAccumulator = 0.0f;
	f32 mSpeed = 0.0f;

public:
	CPP Animation()
	{
		mCurrentFrameNumber = 0;
		mSpeed = 1.0f;
		mTimeAccumulator = 0.0f;
	}
	CPP ~Animation() override {}

	/**
	* Static method that creates an animation.
	* /param frameCount The number of frames of the animation.
	* /param horizontal True if horizontal (left to right), False if vertical (down to up).
	* /param reverse True to use the reverse direction.
	* /param startPosition The start position (bottom-left corner of the frame) in texture coordinates (see OpenGL texture coordinates).
	* /param width The frame width.
	* /param height The frame height.
	* /param speed The speed of the animation.
	* /returns The animation.
	*/
	CPP static Animation create(u32 frameCount, bool horizontal, bool reverse, const Vector2& startPosition, f32 width, f32 height, f32 speed)
	{
		// TODO: check if coordinates are > 1 and < 0 !!!!!

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

	CPP void init()
	{
		mFrames.clear();
	}

	CPP u32 getNumberOfFrames() const
	{
		return mFrames.size();
	}

	CPP const AnimationFrame& getNextFrame()
	{
		// speed -> frame/second.
		// time -> time of one frame.

		f32 time = (1.0 / (mSpeed)) * 1000.0f; // in milliseconds !

		mTimeAccumulator += Time::getInstance().getDeltaTimeMillis();

		//ECHO("RENDERER NUM")

		// if delta time is greater than 'one frame time'
		// then -> change to the next frame.
		if (mTimeAccumulator >= time)
		{
			mTimeAccumulator = 0.0f;
			mCurrentFrameNumber = (mCurrentFrameNumber + 1) % mFrames.size();
		}

		return mFrames.at(mCurrentFrameNumber);
	}

	CPP const AnimationFrame& getCurrentFrame() const
	{
		return mFrames.at(mCurrentFrameNumber);
	}

    COPY(Animation)
    {
		DO_COPY(mName);
        DO_COPY(mSpeed);

        FOR_LIST(it, other->mFrames)
        {
            this->mFrames.push_back(*it);
        }
    }

	CPP void serialize(JSON& json) const
	{
		DO_SERIALIZE("name", mName);
		DO_SERIALIZE("speed", mSpeed);
		DO_SERIALIZE_LIST("frames", mFrames);
	}

	CPP void deserialize(const JSON& json)
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

	CRGET_SET(Name)
	CRGET(Frames)
	GET_SET(Speed)
};