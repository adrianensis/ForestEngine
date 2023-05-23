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
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    Ptr<const Material> mMaterial;
};

class Renderer: SUPER(ComponentWithData<RendererData>)
{
    GENERATE_METADATA(Renderer)
	DECLARE_SERIALIZATION()
	
public:
    ClassId getEngineSystemComponentId() const override { return getClassIdStatic(); }
    
    void init(RendererData& data) override;
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;
    bool hasValidChunk() const;
    
private:
    void updateTextureAnimation();

public:

    Ptr<const Batch> mBatch;
    Ptr<const Chunk> mChunk;
    Matrix4 mRendererModelMatrix;

    // TODO: move to static data? This is incompatible with batching.
    Vector4 mColor;
    Rectangle mTextureRegion;

    // TODO: move to material?
    bool mInvertAxisX = false;
    i32 mDepth = 0;
    bool mUseDepth = false; // overrides Z with Depth

private:
    // TODO: move to static data? This is incompatible with batching.
    Vector3 mPositionOffset;

    TransformState mTransformState;
    std::vector<Vector3> mVertices;
	OwnerPtr<Mesh> mMeshInstance;

    // TODO: move to material?
    std::unordered_map<std::string, Ptr<TextureAnimation>> mTextureAnimations;

public:
    CGET(MeshInstance)
	SET_DIRTY(PositionOffset)
	HASVALID(Batch)
	GET_SET_MAP_CURRENT(TextureAnimations)
};
