#pragma once

#include "Core/Module.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"

class TextureAnimation: public ObjectBase
{
    GENERATE_METADATA(TextureAnimation)
	DECLARE_SERIALIZATION()
	
public:
    TextureAnimation();
    ~TextureAnimation() override;

	/**
	* Static method that creates an TextureAnimation.
	* /param frameCount The number of frames of the TextureAnimation.
	* /param horizontal True if horizontal (left to right), False if vertical (down to up).
	* /param reverse True to use the reverse direction.
	* /param startPosition The start position (bottom-left corner of the frame) in texture coordinates (see OpenGL texture coordinates).
	* /param width The frame width.
	* /param height The frame height.
	* /param speed The speed of the TextureAnimation.
	* /returns The TextureAnimation.
	*/
    static TextureAnimation create(u32 frameCount, bool horizontal, bool reverse, const Vector2& startPosition, f32 width, f32 height, f32 speed);

    void init();
    u32 getNumberOfFrames() const;
    const TextureAnimationFrame& nextFrame();
    const TextureAnimationFrame& getCurrentFrame() const;
	
private:
	u32 mCurrentFrameNumber = 0;
	f32 mTimeAccumulator = 0.0f;
public:
	std::string mName;
	std::vector<TextureAnimationFrame> mFrames;
	f32 mSpeed = 0.0f;
};
