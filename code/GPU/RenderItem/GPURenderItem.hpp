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
    std::unordered_set<GPURenderPassID> mRenderPassIDs;
};

class GPURenderItem
{
public:
    void init(const GPURenderItemData& data, bool isStatic, GPUShaderManager* gpuShaderManager);
    void terminate(GPUShaderManager* gpuShaderManager);
    bool isStatic() const { return mIsStatic; } 

private:
    GPURenderItemData mGPURenderItemData;
    GPU::u32 mInstanceSlot = 0;
    GPU::u32 mRenderSlot = 0;
    Maths::Matrix4 mRendererModelMatrix;
    GPUShaderPropertiesInstance* mGPUShaderPropertiesInstance = nullptr;
    bool mIsStatic = false;
    bool mUpdateMatrix = false;

public:
    CRGET_SET(RendererModelMatrix)
    CRGET(GPURenderItemData)
    RGET(GPUShaderPropertiesInstance)
    CRGET_SET(RenderSlot)
    GET_SET(InstanceSlot)
    SET(IsStatic)
    GET_SET(UpdateMatrix)
};

