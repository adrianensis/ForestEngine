#include "Graphics/Renderer/SpriteRenderer.hpp"

void SpriteRenderer::preUpdate()
{
    updateTextureRegion();
}

void SpriteRenderer::updateTextureRegion()
{
	if (mComponentData.mMaterial.isValid())
	{
		const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();

		if (currentTextureAnimation and !currentTextureAnimation->mFrames.empty())
		{
            mCurrentTextureAnimationUpdater.setTextureAnimation(*currentTextureAnimation);
			const TextureAnimationFrame& frame = mCurrentTextureAnimationUpdater.nextFrame();
			if(mCurrentTextureAnimationUpdater.isNewFrame())
            {
                mTextureRegion.setLeftTopFront(frame.mPosition);
                mTextureRegion.setSize(Vector2(frame.mWidth, frame.mHeight));
                mRegenerateTextureCoords = true;
            }
		}
	}
};

void SpriteRenderer::updatePositions()
{
    MeshRenderer::updatePositions();

    if(mUseDepth)
    {
        FOR_RANGE(i, 0, mMeshInstance->mPositions.size())
        {
            Vector3 vertexPosition = mMeshInstance->mPositions[i];
            vertexPosition.z = mDepth;
            mMeshInstance->mPositions[i] = vertexPosition;
        }
    }
}

void SpriteRenderer::updateTextureCoords()
{
    MeshRenderer::updateTextureCoords();
    
    FOR_RANGE(i, 0, mMeshInstance->mVertexCount)
    {
        Vector2 vertexTexture = mComponentData.mMesh->mTextureCoordinates[i];
        Vector2 regionSize = mTextureRegion.getSize();
        Vector2 regionPosition = mTextureRegion.getLeftTopFront();

        Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

        if (mInvertAxisX)
        {
            textureCoord.x = 1.0f - textureCoord.x;

            const TextureAnimation* currentTextureAnimation = getCurrentTextureAnimation();

            if (currentTextureAnimation)
            {
                textureCoord.x = textureCoord.x - (1.0f - (currentTextureAnimation->getNumberOfFrames() * regionSize.x));
            }
        }

        mMeshInstance->mTextureCoordinates[i] = textureCoord;
    }
}

const TextureAnimation* SpriteRenderer::getCurrentTextureAnimation() const
{
	const TextureAnimation* currentTextureAnimation = nullptr;
    if (mComponentData.mMaterial.isValid())
	{
        const auto& textureAnimationsMap = mComponentData.mMaterial->getMaterialData().mTextureAnimations;
		if (MAP_CONTAINS(textureAnimationsMap, mCurrentTextureAnimationKey))
		{
			currentTextureAnimation = &textureAnimationsMap.at(mCurrentTextureAnimationKey);
		}
	}

    return currentTextureAnimation;
}

void SpriteRenderer::setDepth(i32 depth)
{
    if(mDepth != depth)
    {
        mDepth = depth;
        mRegeneratePositions = true;
    }
}

void SpriteRenderer::setInvertAxisX(bool invertAxisX)
{
    if(mInvertAxisX != invertAxisX)
    {
        mInvertAxisX = invertAxisX;
        mRegenerateTextureCoords = true;
    }
}

void SpriteRenderer::setTextureRegion(const Rectangle& textureRegion)
{
    mTextureRegion = textureRegion;
    mRegenerateTextureCoords = true;
}