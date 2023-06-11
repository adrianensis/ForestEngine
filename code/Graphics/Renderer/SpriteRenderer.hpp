#pragma once

#include "Graphics/Renderer/MeshRenderer.hpp"

class SpriteRenderer: public MeshRenderer
{
    GENERATE_METADATA(SpriteRenderer)
	
private:
    void preUpdate() override { updateTextureRegion(); }
    void updateTextureRegion();
    void updatePositions() override;
    void updateTextureCoords() override;
    const TextureAnimation* getCurrentTextureAnimation() const;

public:
    std::string mCurrentTextureAnimationKey;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    i32 mDepth = 0;
    bool mUseDepth = false; // overrides Z with Depth
    bool mInvertAxisX = false;
    Rectangle mTextureRegion = Rectangle(Vector2(0.0, 0.0), Vector2(1.0, 1.0));

public:
	CRGET_SET(CurrentTextureAnimationKey)
};
