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

void Renderer::update(bool regenerateVertices)
{
	PROFILER_CPU()

	TransformState currentTransformState = TransformState(mGameObject->mTransform.get());

	bool transformChanged = !currentTransformState.eq(mTransformState);

	bool isAnimated = false;
	if(getComponentData().mMesh->mModel.isValid())
	{
		isAnimated = getComponentData().mMesh->mModel->isAnimated();
	}

	u32 verticesCount = getComponentData().mMesh->mVertexCount;

	if (transformChanged || mDirtyPositionOffset || isAnimated)
	{
		if (transformChanged || mDirtyPositionOffset)
		{
			mRendererModelMatrix.translation(mPositionOffset);
			mRendererModelMatrix.mul(mGameObject->mTransform->getModelMatrix());
		}

        if(regenerateVertices)
		{
            mMeshInstance->init(getComponentData().mMesh->mVertexCount, getComponentData().mMesh->mFacesCount);

			if(mVertices.size() < verticesCount)
			{
				mVertices.resize(verticesCount);
			}
			
			FOR_RANGE(i, 0, verticesCount)
			{
				Vector3 vertexPosition = getComponentData().mMesh->mPositions[i];

				if(isAnimated)
				{
					const u32 MAX_BONE_INFLUENCE = smMaxBonesPerVertex;
					const u32 MAX_BONES = 50;

					const std::vector<Matrix4>& boneTransforms = GET_SYSTEM(AnimationManager).getBoneTransforms(getComponentData().mMesh->mModel);
					const std::vector<BoneVertexIDsData>& bonesVertexIDsData = getComponentData().mMesh->mBonesVertexIDsData;
					const std::vector<BoneVertexWeightsData>& boneVertexWeightsData = getComponentData().mMesh->mBonesVertexWeightsData;

					Vector4 skinnedVertexPosition = Vector4(0,0,0,0);
					for(u32 boneIt = 0 ; boneIt < (i32)MAX_BONE_INFLUENCE ; boneIt++)
					{
						const BoneVertexIDsData& IDsData = bonesVertexIDsData[i];
						const BoneVertexWeightsData& wightsData = boneVertexWeightsData[i];
						if(IDsData.mBonesIDs[boneIt] > -1)
						{
							if(IDsData.mBonesIDs[boneIt] >= (i32)MAX_BONES) 
							{
								skinnedVertexPosition = vertexPosition;
								//ASSERT_MSG(false, "TODO: review MAX_BONES!");
								break;
							}

							Vector4 localPosition = boneTransforms[IDsData.mBonesIDs[boneIt]].mulVector(Vector4(vertexPosition,1.0f));
							skinnedVertexPosition += localPosition * wightsData.mBonesWeights[boneIt];
						}
					}

					// NOTE: normalize by w! this is bc we cannot assume w == 1 after bone transforms multiplications
					if(skinnedVertexPosition.w > 0)
					{
						skinnedVertexPosition = skinnedVertexPosition / skinnedVertexPosition.w;
					}

					vertexPosition = mRendererModelMatrix.mulVector(skinnedVertexPosition);
				}
				else
				{
					vertexPosition = mRendererModelMatrix.mulVector(Vector4(vertexPosition, 1));
				}

				if(getComponentData().mUseDepth)
				{
					vertexPosition.z = getComponentData().mDepth;
				}

				mVertices[i] = vertexPosition;
			}

            mMeshInstance->appendToPositions(mVertices);
            //mMeshInstance->appendToTextureCoordinates(mMesh->mTextureCoordinates);

            FOR_RANGE(i, 0, verticesCount)
            {
                Vector2 vertexTexture = getComponentData().mMesh->mTextureCoordinates[i];
                Vector2 regionSize = mTextureRegion.getSize();
                Vector2 regionPosition = mTextureRegion.getLeftTopFront();

                Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

                if (getComponentData().mInvertAxisX)
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
                    
            if(isAnimated)
            {
                mMeshInstance->appendToBonesVertexIDsData(getComponentData().mMesh->mBonesVertexIDsData);
                mMeshInstance->appendToBonesVertexWeightsData(getComponentData().mMesh->mBonesVertexWeightsData);
            }
		}

		mDirtyPositionOffset = false;

		mTransformState = currentTransformState;
	}

    mMeshInstance->setColor(mColor);

	updateTextureAnimation();

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
	SERIALIZE("depth", getComponentData().mDepth)


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