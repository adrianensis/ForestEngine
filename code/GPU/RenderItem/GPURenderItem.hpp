#pragma once

#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPURenderItemData
{
public:
    GPUDepthStencilData mGPUDepthStencilData;
    Core::WeakPtr<const GPUMesh> mMesh;
    Maths::Matrix4 mMeshInstanceMatrix = Maths::Matrix4::smIdentity;
    Core::WeakPtr<GPUShader> mShader;
    std::unordered_set<Core::ClassId> mRenderPassIDs;
};

class GPURenderItem
{
public:
    void init(const GPURenderItemData& data, bool isStatic, GPUShaderManager* gpuShaderManager);
    void terminate(GPUShaderManager* gpuShaderManager);
    bool isStatic() const { return mIsStatic; } 

private:
    GPURenderItemData mGPURenderItemData;
    Core::Slot mInstanceSlot;
    Core::Slot mRenderSlot;
    Maths::Matrix4 mRendererModelMatrix;
    Core::WeakPtr<GPUShaderPropertiesInstance> mGPUShaderPropertiesInstance;
    bool mIsStatic = false;
    bool mUpdateMatrix = false;

public:
    CRGET_SET(RendererModelMatrix)
    CRGET(GPURenderItemData)
    RGET(GPUShaderPropertiesInstance)
    CRGET_SET(RenderSlot)
    CRGET_SET(InstanceSlot)
    SET(IsStatic)
    GET_SET(UpdateMatrix)
};

