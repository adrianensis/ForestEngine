#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Material.hpp"

class Chunk;
class Mesh;
class Batch;

class Renderer: public Component
{
    GENERATE_METADATA(Renderer)
	DECLARE_SERIALIZATION()
	
public:
    void init() override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update(bool regenerateVertices);
    void onDestroy() override;
    bool hasValidChunk() const;
    
private:
    void updateTextureAnimation();

public:
	TransformState mTransformState;
	Matrix4 mRendererModelMatrix;
	std::vector<Vector3> mVertices;
	Vector4 mColor;
	Vector3 mPositionOffset;
	Rectangle mTextureRegion;
	Rectangle mClipRectangle;
	bool mInvertAxisX = false;
	bool mIsLineMode = false;
	i32 mDepth = 0;
	bool mUseDepth = false; // overrides Z with Depth
	u32 mStencilValue = 0x00;
	u32 mStencilFunction = GL_NOTEQUAL;
	bool mIsStencilMask = false;
	f32 mRenderDistance = 0.0f;
	std::unordered_map<std::string, Ptr<TextureAnimation>> mTextureAnimations;
	Ptr<const Chunk> mChunk;
	bool mIsInstanced = false;
	Ptr<const Batch> mBatch;
	Ptr<const Mesh> mMesh;
	Ptr<const Material> mMaterial;

	SET_DIRTY(PositionOffset)
	
	HASVALID(Batch)
	GET_SET_MAP_CURRENT(TextureAnimations)

private:
	bool mPositionsDirty = true;
	SharedPtr<Mesh> mMeshInstance;
public:
    CGET(MeshInstance)
};
