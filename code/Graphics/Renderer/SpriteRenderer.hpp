#pragma once

#include "Graphics/Renderer/MeshRenderer.hpp"

class SpriteRenderer: public MeshRenderer
{
    GENERATE_METADATA(SpriteRenderer)
	
public:
    void setDepth(i32 depth);
    void setInvertAxisX(bool invertAxisX);
    void setTextureRegion(const Rectangle& textureRegion);

private:
    void preUpdate() override;
    void updateTextureRegion();
    void updatePositions() override;
    void updateTextureCoords() override;
    const TextureAnimation* getCurrentTextureAnimation() const;

public:
    std::string mCurrentTextureAnimationKey;
    bool mUseDepth = false; // overrides Z with Depth

private:
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    Rectangle mTextureRegion = Rectangle(Vector2(0.0, 0.0), Vector2(1.0, 1.0));
    bool mInvertAxisX = false;
    i32 mDepth = 0;

public:
	CRGET_SET(CurrentTextureAnimationKey)
};
