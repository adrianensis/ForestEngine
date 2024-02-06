#pragma once

#include "Scene/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/GPU/GPUSharedContext.hpp"

class BatchRenderer;

class StencilData
{
public:
    bool mUseStencil = false;
    // aka ref
    u32 mStencilValue = 0;
    u32 mStencilFunction = GL_NOTEQUAL;
    u32 mStencilPassOp = GL_KEEP;
    u64 mParentId = 0;
    u64 mId = 0;

    bool operator==(const StencilData& other) const
	{
        return
        mUseStencil == other.mUseStencil and
        mStencilValue == other.mStencilValue and
        mParentId == other.mParentId and
        mId == other.mId && 
        mStencilFunction == other.mStencilFunction;
	}
};

class RendererData
{
public:
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    Ptr<const Material> mMaterial;
};

class MeshRenderer: public Component, public IOcTreeElement
{
    GENERATE_METADATA(MeshRenderer)
	DECLARE_SERIALIZATION()
	
public:
    ClassId getSystemComponentId() const override { return getClassDefinitionStatic().mId; }

    void init(const RendererData& data);
    void onComponentAdded() override;
    bool getIsWorldSpace() const;
    void update();
    void onDestroy() override;

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    RendererData mRendererData;
	OwnerPtr<Mesh> mMeshInstance;
    Ptr<BatchRenderer> mBatchRenderer;
    GPUInstanceSlot mGPUInstanceSlot; 
    Matrix4 mRendererModelMatrix;
    bool mRendererPositionOffsetDirty = true;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    MaterialInstance mMaterialInstance;

public:
    std::string mCurrentTextureAnimationKey;

public:
    CGET(MeshInstance)
    CRGET_SET(BatchRenderer)
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
    RGET(MaterialInstance)
    RGET_SET(GPUInstanceSlot)
};
