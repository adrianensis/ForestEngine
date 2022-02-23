#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh.hpp"

class Chunk;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)
	DECL_SERIALIZATION()
	
	public:
	void init() override;
	void onComponentAdded() override;
	void onDestroy() override;
	void update();
	void setPositionOffset (const Vector3& newPositionOffset);
	bool getIsWorldSpace() const;
	bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }
	const Animation* getCurrentAnimation() const { return &mAnimations.at(mCurrentAnimationName); }

	private:
	void updateAnimation();

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
	std::map<std::string, Animation> mAnimations;
	std::string mCurrentAnimationName;
	Chunk* mChunk = nullptr;
	Batch* mBatch = nullptr;
	const Mesh* mMesh = nullptr;
	Material* mMaterial = nullptr;

	public:
	GET_RC_SET(RendererModelMatrix)
	GET_RC_SET(Vertices)
	GET_RC_SET(Color)
	GET(PositionOffset)
	GET_RC_SET(TextureRegion)
	GET_RC_SET(ClipRectangle)
	GET_SET(InvertAxisX)
	GET_SET(IsLineMode)
	GET_SET(Depth)
	GET_SET(UseDepth)
	GET_SET(RenderDistance)
	GET_R(Animations)
	GET_SET(CurrentAnimationName)
	GET_SET(Chunk)
	GET_SET(Batch)
	GET_SET(Mesh)
	GET_SET(Material)
};