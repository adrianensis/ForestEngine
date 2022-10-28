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

	setColor(Vector4(0, 0, 0, 1));

	mDirtyPositionOffset = true;
}

void Renderer::onComponentAdded() 
{
	mTransformState = TransformState(getGameObject()->getTransform().get());

	//update();
}

bool Renderer::getIsWorldSpace() const
{
	return getGameObject()->getTransform().get().getAffectedByProjection();
}

void Renderer::update(bool regenerateVertices)
{
	PROFILER_CPU()

	TransformState currentTransformState = TransformState(getGameObject()->getTransform().get());

	bool transformChanged = !currentTransformState.eq(mTransformState);

	bool isAnimated = false;
	if(mMesh.get().getModel().isValid())
	{
		isAnimated = mMesh.get().getModel().get().isAnimated();
	}

	if (transformChanged || mDirtyPositionOffset || isAnimated)
	{
		if (transformChanged || mDirtyPositionOffset)
		{
			mRendererModelMatrix.translation(mPositionOffset);
			mRendererModelMatrix.mul(getGameObject()->getTransform().get().getModelMatrix());
		}

		if(regenerateVertices)
		{
			u32 verticesCount = mMesh.get().getVertexCount();

			if(mVertices.size() < verticesCount)
			{
				mVertices.clear();
				mVertices.reserve(verticesCount);
			}
			
			FOR_RANGE(i, 0, verticesCount)
			{
				Vector3 vertexPosition = mMesh.get().getPositions()[i];

				if(isAnimated)
				{
					const u32 MAX_BONE_INFLUENCE = BoneVertexData::smMaxBonesPerVertex;
					const u32 MAX_BONES = 100;

					const std::vector<Matrix4>& boneTransforms = AnimationManager::getInstance().getBoneTransforms(mMesh.get().getModel());
					const std::vector<BoneVertexData>& bonesVertexData = mMesh.get().getBonesVertexData();

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
	if(mBatch.isValid())
	{
		mBatch.get().forceRegenerateBuffers();
	}
}

const Mesh& Renderer::generateMeshInstance()
{
	const std::vector<Vector3>& vertexPositions = getVertices();

	mMeshInstance.init(mMesh.get().getVertexCount(), mMesh.get().getFacesCount());

	FOR_RANGE(i, 0, mMesh.get().getVertexCount())
	{
		mMeshInstance.addPosition(vertexPositions[i]);

		Vector2 vertexTexture = mMesh.get().getTextureCoordinates()[i];

		Vector2 regionSize = getTextureRegion().getSize();
		Vector2 regionPosition = getTextureRegion().getLeftTopFront();

		Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

		if (getInvertAxisX())
		{
			textureCoord.x = 1.0f - textureCoord.x;

			Ptr<const TextureAnimation> TextureAnimation = getTextureAnimationsCurrent();

			if (TextureAnimation)
			{
				textureCoord.x = textureCoord.x - (1.0f - (TextureAnimation.get().getNumberOfFrames() * regionSize.x));
			}
		}

		mMeshInstance.addTexCoord(textureCoord);

		mMeshInstance.addColor(getColor());
	}

	mMeshInstance.copyBones(mMesh);

	return mMeshInstance;
}

bool Renderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() && mChunk.get().getIsLoaded()); // !chunk means -> Screen Space case
}

IMPLEMENT_SERIALIZATION(Renderer)
{
	Component::serialize(json);

	std::string materialPath = "";

	if(mMaterial.get().getTexture())
	{
		materialPath = mMaterial.get().getTexture().get().getPath();
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

		if (currentTextureAnimation && !currentTextureAnimation.get().getFrames().empty())
		{
			const TextureAnimationFrame& frame = currentTextureAnimation.get().getNextFrame();
			mTextureRegion.setLeftTopFront(frame.getPosition());
			mTextureRegion.setSize(Vector2(frame.getWidth(), frame.getHeight()));
		}
	}
};