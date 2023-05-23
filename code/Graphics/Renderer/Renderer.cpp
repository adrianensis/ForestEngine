#include "Graphics/Renderer/Renderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/Shaders/Shader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Scene/Module.hpp"

void Renderer::init(RendererData& data) 
{
    ComponentWithData::init(data);
	mTextureRegion.setLeftTopFront(Vector2(0.0, 0.0));
	mTextureRegion.setSize(Vector2(1.0, 1.0));

	mColor = (Vector4(0, 0, 0, 1));

	mDirtyPositionOffset = true;

    mMeshInstance = OwnerPtr<Mesh>::newObject();
}

void Renderer::onComponentAdded() 
{
	mTransformState = TransformState(mGameObject->mTransform.get());
}

bool Renderer::getIsWorldSpace() const
{
	return mGameObject->mTransform->mAffectedByProjection;
}

void Renderer::update()
{
	PROFILER_CPU()

	updateTextureAnimation();

	TransformState currentTransformState = TransformState(mGameObject->mTransform.get());
	bool transformChanged = !currentTransformState.eq(mTransformState);

	u32 verticesCount = getComponentData().mMesh->mVertexCount;

	if (transformChanged || mDirtyPositionOffset)
	{
        mRendererModelMatrix.translation(mPositionOffset);
        mRendererModelMatrix.mul(mGameObject->mTransform->getModelMatrix());

        bool regenerateVertices = !getComponentData().mIsInstanced;
        if(regenerateVertices)
		{
            mMeshInstance->init(getComponentData().mMesh->mVertexCount, getComponentData().mMesh->mFacesCount);

            mVertices = getComponentData().mMesh->mPositions;

            if(mUseDepth)
            {
                FOR_RANGE(i, 0, mVertices.size())
                {
                    Vector3 vertexPosition = mVertices[i];
                    vertexPosition.z = mDepth;
                    mVertices[i] = vertexPosition;
                }
            }

            mMeshInstance->appendToPositions(mVertices);

            FOR_RANGE(i, 0, verticesCount)
            {
                Vector2 vertexTexture = getComponentData().mMesh->mTextureCoordinates[i];
                Vector2 regionSize = mTextureRegion.getSize();
                Vector2 regionPosition = mTextureRegion.getLeftTopFront();

                Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

                if (mInvertAxisX)
                {
                    textureCoord.x = 1.0f - textureCoord.x;

                    Ptr<const TextureAnimation> TextureAnimation = getTextureAnimationsCurrent();

                    if (TextureAnimation)
                    {
                        textureCoord.x = textureCoord.x - (1.0f - (TextureAnimation->getNumberOfFrames() * regionSize.x));
                    }
                }

                mMeshInstance->addToTextureCoordinates(textureCoord);
            }
                    
            mMeshInstance->appendToBonesVertexIDsData(getComponentData().mMesh->mBonesVertexIDsData);
            mMeshInstance->appendToBonesVertexWeightsData(getComponentData().mMesh->mBonesVertexWeightsData);
		}

		mDirtyPositionOffset = false;

		mTransformState = currentTransformState;
	}

    mMeshInstance->setColor(mColor);
}

void Renderer::onDestroy() 
{

}

bool Renderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() and mChunk->mIsLoaded); // !chunk means -> Screen Space case
}

IMPLEMENT_SERIALIZATION(Renderer)
{
	Component::serialize(json);

	std::string materialPath = "";

//	if(mMaterial->mTexture)
//	{
//		materialPath = mMaterial->mTexture->mPath;
//	}

	SERIALIZE("material", materialPath)
	SERIALIZE("region", mTextureRegion)
	SERIALIZE("depth", mDepth)
}

IMPLEMENT_DESERIALIZATION(Renderer)
{
	std::string materialPath = "";
	DESERIALIZE("material", materialPath)

	//mMaterial = GET_SYSTEM(MaterialManager).loadMaterial(materialPath);

	// DESERIALIZE("region", mTextureRegion)
	// DESERIALIZE("depth", getComponentData().mDepth)
}

void Renderer::updateTextureAnimation()
{
	if (getComponentData().mMaterial.isValid())
	{
		Ptr<TextureAnimation> currentTextureAnimation;
		if (MAP_CONTAINS(mTextureAnimations, mTextureAnimationsCurrentKey))
		{
			currentTextureAnimation = mTextureAnimations[mTextureAnimationsCurrentKey];
		}

		if (currentTextureAnimation and !currentTextureAnimation->mFrames.empty())
		{
			const TextureAnimationFrame& frame = currentTextureAnimation->nextFrame();
			mTextureRegion.setLeftTopFront(frame.mPosition);
			mTextureRegion.setSize(Vector2(frame.mWidth, frame.mHeight));
		}
	}
};