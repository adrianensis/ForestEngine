#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Animation/Animation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

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
	std::map<std::string, Ref<Animation>> mAnimations;
	std::string mCurrentAnimationName;
	Ref<Chunk> mChunk;
	Ref<Batch> mBatch;
	Ref<const Mesh> mMesh;
	Mesh mMeshInstance;
	Ref<Material> mMaterial;

private:
	void updateAnimation();

public:
	void init() override;
	void onComponentAdded() override;
	void onDestroy() override;
	void update();
	void setPositionOffset (const Vector3& newPositionOffset);
	bool getIsWorldSpace() const;
	bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }
	Ref<const Animation> getCurrentAnimation() const { return mAnimations.at(mCurrentAnimationName); }

	const Mesh& generateMeshInstance();

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

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
};