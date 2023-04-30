#pragma once

#include "Scene/Component.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/TextureAnimation/TextureAnimation.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"

class Chunk;
class Batch;

class StencilData
{
public:
    u32 mStencilValue = 0x00;
    u32 mStencilFunction = GL_NOTEQUAL;
    bool mIsStencilMask = false;
};

class RendererData: public ComponentData
{
public:
    bool mInvertAxisX = false;
    i32 mDepth = 0;
    bool mUseDepth = false; // overrides Z with Depth
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    Ptr<const Material> mMaterial;
};

class Renderer: public ComponentWithData<RendererData>
{
    GENERATE_METADATA(Renderer)
	DECLARE_SERIALIZATION()
	
public:
    void init(RendererData& data) override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update(bool regenerateVertices);
    void onDestroy() override;
    bool hasValidChunk() const;
    
private:
    void updateTextureAnimation();

public:

    Ptr<const Batch> mBatch;
    Ptr<const Chunk> mChunk;
    Matrix4 mRendererModelMatrix;
    Vector4 mColor;
    Rectangle mTextureRegion;
    
private:

    Vector3 mPositionOffset;
    TransformState mTransformState;
    std::vector<Vector3> mVertices;
	bool mPositionsDirty = true;
	OwnerPtr<Mesh> mMeshInstance;

    // TODO: move to material?
    std::unordered_map<std::string, Ptr<TextureAnimation>> mTextureAnimations;

public:
    CGET(MeshInstance)
	SET_DIRTY(PositionOffset)
	HASVALID(Batch)
	GET_SET_MAP_CURRENT(TextureAnimations)
};
