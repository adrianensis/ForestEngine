#pragma once

#include "Scene/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Mesh/Mesh.hpp"
#include "Graphics/RenderSharedContext.hpp"

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

    bool matches(const StencilData& other) const
    {
        return
        mUseStencil == other.mUseStencil and
        mStencilValue == other.mStencilValue and
        mParentId == other.mParentId and
        //mId == other.mId && 
        mStencilFunction == other.mStencilFunction;
    }

    u64 hash() const
    {
        return (u64)mUseStencil^
        (u64)mStencilValue ^ static_cast<u64>(mStencilFunction) ^
        (u64)mParentId /*^ (u64)mId*/;
    }
};

class RendererData
{
public:
    StencilData mStencilData;
    bool mIsInstanced = false;
    Ptr<const Mesh> mMesh;
    PoolHandler<Material> mMaterial;
    std::unordered_set<ClassId> mRenderPassIDs;
};

class MeshRenderer: public Component, public IOcTreeElement
{
    
	DECLARE_SERIALIZATION()
	
public:
    ClassId getSystemComponentId() const override;

    void init(const RendererData& data);
    void onComponentAdded() override;
    void onDestroy() override;
    void update();

private:
    void calculateRendererModelMatrix();
    const TextureAnimation* getCurrentTextureAnimation() const;
    void updateTextureRegion();

private:
    RendererData mRendererData;
	OwnerPtr<Mesh> mMeshInstance;
    Ptr<BatchRenderer> mBatchRenderer;
    Slot mRenderInstanceSlot;
    Slot mMaterialInstanceSlot;
    Matrix4 mRendererModelMatrix;
    bool mRendererPositionOffsetDirty = true;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    MaterialInstance mMaterialInstance;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    CGET(MeshInstance)
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
    RGET(MaterialInstance)
    RGET(RenderInstanceSlot)
    RGET(MaterialInstanceSlot)
};
REGISTER_CLASS(MeshRenderer);
