#pragma once

#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPURenderItemData
{
public:
    GPUDepthStencilData mGPUDepthStencilData;
    const GPUMesh* mMesh = nullptr;
    Maths::Matrix4 mMeshInstanceMatrix = Maths::Matrix4::smIdentity;
    GPUShader* mShader = nullptr;
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
    GPUShaderPropertiesInstance* mGPUShaderPropertiesInstance = nullptr;
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

