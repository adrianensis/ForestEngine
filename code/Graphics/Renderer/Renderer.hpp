#ifndef RENDERER_HPP
#define RENDERER_HPP

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
	
public:

	bool hasClipRectangle() const { return mClipRectangle.getSize().len() > MathUtils::FLOAT_EPSILON; }
	Ptr<const Animation> getCurrentAnimation() const { return mAnimations.at(mCurrentAnimationName); }

    void init() override;
    void onComponentAdded() override;
    void setPositionOffset(const Vector3& newPositionOffset);
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;
    const Mesh& generateMeshInstance();
    bool hasValidChunk() const;
    void serialize(JSON& json) const override;
    void deserialize(const JSON& json) override;

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
	bool mUseDepth = false; // overrides Z with Depth
	f32 mRenderDistance = 0.0f;
	std::map<std::string, Ptr<Animation>> mAnimations;
	std::string mCurrentAnimationName;
	Ptr<Chunk> mChunk;
	Ptr<Batch> mBatch;
	Ptr<const Mesh> mMesh;
	Mesh mMeshInstance;
	Ptr<Material> mMaterial;

public:
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
	SET(Chunk)
	SET(Batch)
	HASVALID(Batch)
	RGET_SET(Mesh)
	RGET_SET(Material)
};

#endif