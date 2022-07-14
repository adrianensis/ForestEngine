#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class Chunk;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)
	DECLARE_SERIALIZATION()
	
public:
    void init() override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;
    CR(Mesh) generateMeshInstance();
    bool hasValidChunk() const;

private:
    void updateTextureAnimation();

private:
	TransformState mTransformState;
	Matrix4 mRendererModelMatrix;
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
	std::map<std::string, Ptr<TextureAnimation>> mTextureAnimations;
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
	SET_DIRTY(PositionOffset)
	CRGET_SET(TextureRegion)
	CRGET_SET(ClipRectangle)
	GET_SET(InvertAxisX)
	GET_SET(IsLineMode)
	GET_SET(Depth)
	GET_SET(UseDepth)
	GET_SET(RenderDistance)
	SET(Chunk)
	SET(Batch)
	HASVALID(Batch)
	GET_SET(Mesh)
	GET_SET(Material)
	MAP_GETCURRENT(TextureAnimations)
};

#endif