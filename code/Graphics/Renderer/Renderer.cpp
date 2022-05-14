#include "Graphics/Renderer/Renderer.hpp"

#include "Graphics/Animation/AnimationFrame.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Mesh/MeshPrimitives.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Scene/Module.hpp"


void Renderer::init() 
{

	mTextureRegion.setLeftTopFront(Vector2(0.0, 0.0));
	mTextureRegion.setSize(Vector2(1.0, 1.0));

	mRenderDistance = 1500; // TODO : move to settings?

	setColor(Vector4(0, 0, 0, 1));
}

void Renderer::onComponentAdded() 
{
	mTransformState = TransformState(getGameObject()->getTransform().get());

	update();
}

void Renderer::setPositionOffset(const Vector3& newPositionOffset)
{
	mPositionOffset = newPositionOffset;
	mVerticesDirty = true;
	mRendererModelMatrixGenerated = false;
};

bool Renderer::getIsWorldSpace() const
{
	return getGameObject()->getTransform().get().getAffectedByProjection();
}

void Renderer::update()
{
	TransformState currentTransformState = TransformState(getGameObject()->getTransform().get());

	bool transformChanged = !currentTransformState.eq(mTransformState);

	if (transformChanged || (!mRendererModelMatrixGenerated))
	{
		mRendererModelMatrix.translation(mPositionOffset);
		mRendererModelMatrix.mul(getGameObject()->getTransform().get().getModelMatrix());

		mRendererModelMatrixGenerated = true;
	}

	if (transformChanged || mVerticesDirty)
	{
		u32 verticesCount = mMesh.get().getVertexCount();

		if(mVertices.size() < verticesCount)
		{
			mVertices.clear();
			mVertices.reserve(verticesCount);
		}
		
		FOR_RANGE(i, 0, verticesCount)
		{
			Vector3 vertexPosition(
				mMesh.get().getVertices()[i * 3 + 0],
				mMesh.get().getVertices()[i * 3 + 1],
				mMesh.get().getVertices()[i * 3 + 2]);

			vertexPosition = mRendererModelMatrix.mulVector(Vector4(vertexPosition, 1));

			if(mUseDepth)
			{
				vertexPosition.z = mDepth;
			}

			mVertices[i] = vertexPosition;
		}

		mVerticesDirty = false;
	}

	if(transformChanged)
	{
		mTransformState = currentTransformState;
	}

	updateAnimation();
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
	const std::vector<Vector3> &vertexPositions = getVertices();

	mMeshInstance.init(mMesh.get().getVertexCount(), mMesh.get().getFacesCount());

	FOR_RANGE(i, 0, mMesh.get().getVertexCount())
	{
		mMeshInstance.addVertex(vertexPositions[i]);

		Vector2 vertexTexture(
			mMesh.get().getTextureCoordinates()[i * Mesh::smVertexTexCoordSize + 0],
			mMesh.get().getTextureCoordinates()[i * Mesh::smVertexTexCoordSize + 1]);

		Vector2 regionSize = getTextureRegion().getSize();
		Vector2 regionPosition = getTextureRegion().getLeftTopFront();

		Vector2 textureCoord(vertexTexture.x * regionSize.x + regionPosition.x, vertexTexture.y * regionSize.y + regionPosition.y);

		if (getInvertAxisX())
		{
			textureCoord.x = 1.0f - textureCoord.x;

			Ptr<const Animation> animation = getCurrentAnimation();

			if (animation)
			{
				textureCoord.x = textureCoord.x - (1.0f - (animation.get().getNumberOfFrames() * regionSize.x));
			}
		}

		mMeshInstance.addTexCoord(textureCoord.x, textureCoord.y);

		mMeshInstance.addColor(
			getColor().x,
			getColor().y,
			getColor().z,
			getColor().w);
	}

	return mMeshInstance;
}

bool Renderer::hasValidChunk() const
{
	return (! mChunk.isValid()) || (mChunk.isValid() && mChunk.get().getIsLoaded()); // !chunk means -> Screen Space case
}

void Renderer::serialize(JSON& json) const 
{
	Component::serialize(json);

	std::string materialPath = "";

	if(mMaterial.get().getTexture())
	{
		materialPath = mMaterial.get().getTexture().get().getPath();
	}

	DO_SERIALIZE("material", materialPath)
	DO_SERIALIZE("region", mTextureRegion)
	DO_SERIALIZE("depth", mDepth)


}

void Renderer::deserialize(const JSON& json) 
{
	std::string materialPath = "";
	DO_DESERIALIZE("material", materialPath)

	mMaterial = MaterialManager::getInstance().loadMaterial(materialPath);

	DO_DESERIALIZE("region", mTextureRegion)
	DO_DESERIALIZE("depth", mDepth)



}

void Renderer::updateAnimation()
{
	if (mMaterial.isValid())
	{
		Ptr<Animation> currentAnimation;
		if (MAP_CONTAINS(mAnimations, mCurrentAnimationName))
		{
			currentAnimation = mAnimations[mCurrentAnimationName];
		}

		if (currentAnimation && !currentAnimation.get().getFrames().empty())
		{
			const AnimationFrame& frame = currentAnimation.get().getNextFrame();
			mTextureRegion.setLeftTopFront(frame.getPosition());
			mTextureRegion.setSize(Vector2(frame.getWidth(), frame.getHeight()));
		}
	}
};
