#pragma once

#include "Core/ECS/Component.hpp"
#include "SpacePartition/OcTree.hpp"

#include "Scene/Transform.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/GPU/Mesh/GPUMesh.hpp"

class InstancedMeshRenderer;
class MaterialInstance;

class StencilData
{
public:
    bool mUseStencil = false;
    // aka ref
    u32 mStencilValue = 0;
    GPUStencilFunction mStencilFunction = GPUStencilFunction::NOTEQUAL;
    GPUStencilOp mStencilPassOp = GPUStencilOp::KEEP;
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
        u32 shift = 0;
        u64 result = (u64)mUseStencil << (shift++);
        result = result ^ (u64)mStencilValue << (shift++);
        result = result ^ static_cast<u64>(mStencilFunction) << (shift++);
        result = result ^ (u64)mParentId << (shift++); /*^ (u64)mId*/;
        return result;
    }
};

class RendererData
{
public:
    StencilData mStencilData;
    Ptr<const GPUMesh> mMesh;
    Matrix4 mMeshInstanceMatrix = Matrix4::smIdentity;
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
    Slot mInstanceSlot;
    Slot mRenderSlot;
    Matrix4 mRendererModelMatrix;
    TextureAnimationUpdater mCurrentTextureAnimationUpdater;
    PoolHandler<MaterialInstance> mMaterialInstance;
    bool mUpdateMatrix = false;

public:
    HashedString mCurrentTextureAnimationKey;

public:
    CRGET(RendererModelMatrix)
    CRGET(RendererData)
    RGET(MaterialInstance)
    CRGET_SET(RenderSlot)
    CRGET_SET(InstanceSlot)
    GET_SET(UpdateMatrix)
};
REGISTER_CLASS(MeshRenderer);
