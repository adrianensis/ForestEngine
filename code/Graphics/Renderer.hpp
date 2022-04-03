#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

#ifdef CPP_INCLUDE
#include "Graphics/Renderer.hpp"

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
#endif

class Chunk;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)
	
private:
	TransformState mTransformState;
	Matrix4 mRendererModelMatrix;
	bool mRendererModelMatrixGenerated = false;
	bool mVerticesDirty = true;
	std::vector<Vector3> mVertices;
	Vector4 mColor;
	Vector3 mPositionOffset;
	Rectangle mTextureRegion;
	Rectangle mClipRectangle;
	bool mInvertAxisX = false;
	bool mIsLineMode = false;
	i32 mDepth = 0;
	bool mUseDepth = true; // overrides Z with Depth
	f32 mRenderDistance = 0.0f;
	std::map<std::string, Ptr<Animation>> mAnimations;
	std::string mCurrentAnimationName;
	Ptr<Chunk> mChunk;
	Ptr<Batch> mBatch;
	Ptr<const Mesh> mMesh;
	Mesh mMeshInstance;
	Ptr<Material> mMaterial;

private:
	//void updateAnimation();
	
	CPP void updateAnimation()
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
				mTextureRegion.setLeftTop(frame.getPosition());
				mTextureRegion.setSize(Vector2(frame.getWidth(), frame.getHeight()));
			}
		}
	};
public:

	bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }
	Ptr<const Animation> getCurrentAnimation() const { return mAnimations.at(mCurrentAnimationName); }

	CRGET_SET(RendererModelMatrix)
	CRGET_SET(Vertices)
	CRGET_SET(Color)
	GET(PositionOffset)
	CRGET_SET(TextureRegion)
	CRGET_SET(ClipRectangle)
	GET_SET(InvertAxisX)
	GET_SET(IsLineMode)
	GET_SET(Depth)
	GET_SET(UseDepth)
	GET_SET(RenderDistance)
	RGET(Animations)
	GET_SET(CurrentAnimationName)
	RGET_SET(Chunk)
	RGET_SET(Batch)
	RGET_SET(Mesh)
	RGET_SET(Material)

	CPP void init() override
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

	CPP void onComponentAdded() override
	{
		mTransformState = TransformState(getGameObject()->getTransform().get());

		// Force vertices generatiin
		update();
	}

	CPP void setPositionOffset(const Vector3& newPositionOffset)
	{
		mPositionOffset = newPositionOffset;
		mVerticesDirty = true;
		mRendererModelMatrixGenerated = false;
	};

	CPP bool getIsWorldSpace() const
	{
		return getGameObject()->getTransform().get().getAffectedByProjection();
	}

	CPP void update()
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

	CPP void onDestroy() override
	{
		if(mBatch.isValid())
		{
			mBatch.get().forceRegenerateBuffers();
		}
	}

	CPP const Mesh& generateMeshInstance()
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
			Vector2 regionPosition = getTextureRegion().getLeftTop();

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

	CPP void serialize(JSON& json) const override
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

	CPP void deserialize(const JSON& json) override
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
};