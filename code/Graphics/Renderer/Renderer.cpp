#include "Graphics/Renderer/Renderer.hpp"

#include "Graphics/Material/TextureAnimation/TextureAnimationFrame.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Model/Animation/AnimationManager.hpp"
#include "Scene/Module.hpp"

void Renderer::init() 
{
	mTextureRegion.setLeftTopFront(Vector2(0.0, 0.0));
	mTextureRegion.setSize(Vector2(1.0, 1.0));

	mRenderDistance = 1500; // TODO : move to settings?

	mColor = (Vector4(0, 0, 0, 1));

	mDirtyPositionOffset = true;
}

void Renderer::onComponentAdded() 
{
	mTransformState = TransformState(mGameObject->mTransform.get());

	//update();
}

bool Renderer::getIsWorldSpace() const
{
	return mGameObject->mTransform.get().mAffectedByProjection;
}

void Renderer::update(bool regenerateVertices)
{
	PROFILER_CPU()

	TransformState currentTransformState = TransformState(mGameObject->mTransform.get());

	bool transformChanged = !currentTransformState.eq(mTransformState);

	bool isAnimated = false;
	if(mMesh.get().mModel.isValid())
	{
		isAnimated = mMesh.get().mModel.get().isAnimated();
	}

	if (transformChanged || mDirtyPositionOffset || isAnimated)
	{
		if (transformChanged || mDirtyPositionOffset)
		{
			mRendererModelMatrix.translation(mPositionOffset);
			mRendererModelMatrix.mul(mGameObject->mTransform.get().getModelMatrix());
		}

		if(regenerateVertices)
		{
			u32 verticesCount = mMesh.get().mVertexCount;

			if(mVertices.size() < verticesCount)
			{
				mVertices.clear();
				mVertices.reserve(verticesCount);
			}
			
			FOR_RANGE(i, 0, verticesCount)
			{
				Vector3 vertexPosition = mMesh.get().mPositions[i];

				if(isAnimated)
				{
					const u32 MAX_BONE_INFLUENCE = BoneVertexData::smMaxBonesPerVertex;
					const u32 MAX_BONES = 100;

					const std::vector<Matrix4>& boneTransforms = AnimationManager::getInstance().getBoneTransforms(mMesh.get().mModel);
					const std::vector<BoneVertexData>& bonesVertexData = mMesh.get().mBonesVertexData;

					Vector4 skinnedVertexPosition = Vector4(0,0,0,0);
					for(int boneIt = 0 ; boneIt < MAX_BONE_INFLUENCE ; boneIt++)
					{
						const BoneVertexData& boneVertexData = bonesVertexData[i];
						if(boneVertexData.mBoneIDs[boneIt] > -1)
						{
							if(boneVertexData.mBoneIDs[boneIt] >= MAX_BONES) 
							{
								skinnedVertexPosition = vertexPosition;
								//ASSERT_MSG(false, "TODO: review MAX_BONES!");
								break;
							}

							Vector4 localPosition = boneTransforms[boneVertexData.mBoneIDs[boneIt]].mulVector(Vector4(vertexPosition,1.0f));
							skinnedVertexPosition += localPosition * boneVertexData.mBoneWeights[boneIt];
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

				if(mUseDepth)
				{
					vertexPosition.z = mDepth;
				}

				mVertices[i] = vertexPosition;
			}
		}

		mDirtyPositionOffset = false;

		mTransformState = currentTransformState;
	}

	updateTextureAnimation();
}

void Renderer::onDestroy() 
{

}

const Mesh& Renderer::generateMeshInstance()
{
	const std::vector<Vector3>& vertexPositions = mVertices;

	mMeshInstance.init(mMesh.get().mVertexCount, mMesh.get().mFacesCount);

	FOR_RANGE(i, 0, mMesh.get().mVertexCount)
	{
		mMeshInstance.addToPositions(vertexPositions[i]);

		Vector2 vertexTexture = mMesh.get().mTextureCoordinates[i];

		Vector2 regionSize = mTextureRegion.getSize();
		Vector2 regionPosition = mTextureRegion.getLeftTopFront();

		Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

		if (mInvertAxisX)
		{
			textureCoord.x = 1.0f - textureCoord.x;

			Ptr<const TextureAnimation> TextureAnimation = getTextureAnimationsCurrent();

			if (TextureAnimation)
			{
				textureCoord.x = textureCoord.x - (1.0f - (TextureAnimation.get().getNumberOfFrames() * regionSize.x));
			}
		}

		mMeshInstance.addToTextureCoordinates(textureCoord);

		mMeshInstance.addToColors(mColor);
	}

	mMeshInstance.appendToBonesVertexData(mMesh.get().mBonesVertexData);

	return mMeshInstance;
}

bool Renderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() && mChunk.get().mIsLoaded); // !chunk means -> Screen Space case
}

IMPLEMENT_SERIALIZATION(Renderer)
{
	Component::serialize(json);

	std::string materialPath = "";

	if(mMaterial.get().mTexture)
	{
		materialPath = mMaterial.get().mTexture.get().mPath;
	}

	SERIALIZE("material", materialPath)
	SERIALIZE("region", mTextureRegion)
	SERIALIZE("depth", mDepth)


}

IMPLEMENT_DESERIALIZATION(Renderer)
{
	std::string materialPath = "";
	DESERIALIZE("material", materialPath)

	//mMaterial = MaterialManager::getInstance().loadMaterial(materialPath);

	DESERIALIZE("region", mTextureRegion)
	DESERIALIZE("depth", mDepth)



}

void Renderer::updateTextureAnimation()
{
	if (mMaterial.isValid())
	{
		Ptr<TextureAnimation> currentTextureAnimation;
		if (MAP_CONTAINS(mTextureAnimations, mTextureAnimationsCurrentKey))
		{
			currentTextureAnimation = mTextureAnimations[mTextureAnimationsCurrentKey];
		}

		if (currentTextureAnimation && !currentTextureAnimation.get().mFrames.empty())
		{
			const TextureAnimationFrame& frame = currentTextureAnimation.get().getNextFrame();
			mTextureRegion.setLeftTopFront(frame.mPosition);
			mTextureRegion.setSize(Vector2(frame.mWidth, frame.mHeight));
		}
	}
};