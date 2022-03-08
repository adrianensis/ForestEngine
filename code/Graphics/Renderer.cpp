#include "Graphics/Renderer.hpp"

#include "Graphics/Animation/AnimationFrame.hpp"
#include "Graphics/Material/Shader.hpp"
#include "Graphics/RenderEngine.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Graphics/Material/Texture.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/MeshPrimitives.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Batch/Chunk.hpp"
#include "Graphics/Batch/Batch.hpp"
#include "Scene/Module.hpp"

void Renderer::init()
{
	// TRACE();

	// texture region
	mTextureRegion.setLeftTop(Vector2(0.0, 0.0));
	mTextureRegion.setSize(Vector2(1.0, 1.0));

	mRenderDistance = 1500; // TODO : move to settings?

	setColor(Vector4(0, 0, 0, 1));

	mVertices.reserve(4);

	mVertices.push_back(Vector2(0, 0)); // LEFT TOP VERTEX
	mVertices.push_back(Vector2(0, 0)); // LEFT BOTTOM
	mVertices.push_back(Vector2(0, 0)); // RIGHT BOTTOM
	mVertices.push_back(Vector2(0, 0)); // RIGHT TOP
}

void Renderer::onComponentAdded()
{
	mTransformState = TransformState(*getGameObject()->getTransform());

	// Force vertices generatiin
	update();
}

void Renderer::updateAnimation()
{
	if (mMaterial.isValid())
	{
		Ref<Animation> currentAnimation;
		if (MAP_CONTAINS(mAnimations, mCurrentAnimationName))
		{
			currentAnimation = mAnimations[mCurrentAnimationName];
		}

		if (currentAnimation && !currentAnimation.get().getFrames().empty())
		{
			const AnimationFrame& frame = currentAnimation.get().getNextFrame();
			mTextureRegion.setLeftTop(frame.getPosition());
			mTextureRegion.setSize(Vector2(frame.getWidth(), frame.getHeight()));
		}
	}
};

void Renderer::setPositionOffset(const Vector3& newPositionOffset)
{
	mPositionOffset = newPositionOffset;
	mVerticesDirty = true;
	mRendererModelMatrixGenerated = false;
};

bool Renderer::getIsWorldSpace() const
{
	return getGameObject()->getTransform()->getAffectedByProjection();
}

void Renderer::update()
{
	TransformState currentTransformState = TransformState(*getGameObject()->getTransform());

	bool transformChanged = !currentTransformState.eq(mTransformState);

	if (transformChanged || (!mRendererModelMatrixGenerated))
	{
		mRendererModelMatrix.translation(mPositionOffset);
		mRendererModelMatrix.mul(getGameObject()->getTransform()->getModelMatrix());

		mRendererModelMatrixGenerated = true;
	}

	if (transformChanged || mVerticesDirty)
	{
		FOR_ARRAY(i, mVertices)
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

    // std::list<Animation> tmpList;
    // FOR_MAP(it, mAnimations)
    // {
    //     tmpList.push_back(it->second);
    // }

    // DO_SERIALIZE_LIST("animations", tmpList)
}

void Renderer::deserialize(const JSON& json)
{
	std::string materialPath = "";
	DO_DESERIALIZE("material", materialPath)

	mMaterial = MaterialManager::getInstance().loadMaterial(materialPath);

	DO_DESERIALIZE("region", mTextureRegion)
	DO_DESERIALIZE("depth", mDepth)

	//mMesh = MeshPrimitives::getInstance().getPrimitive<Rectangle>();

    // std::list<Animation> tmpList;
    // DO_DESERIALIZE_LIST("animations", tmpList, [](const JSON& json)
    // {
    //     Animation animation;
    //     animation.deserialize(json);
    //     return animation;
    // });

    // FOR_LIST(it, tmpList)
    // {
    //     MAP_INSERT(mAnimations, (*it).getName(), RefRaw<Animation>(*it))
    // }
}