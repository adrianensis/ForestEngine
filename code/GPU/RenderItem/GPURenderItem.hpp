#pragma once

#include "GPU/Mesh/GPUMesh.hpp"
#include "GPU/Shader/GPUShader.hpp"

class GPURenderItemData
{
public:
    GPUShaderStencilData mGPUShaderStencilData;
    WeakPtr<const GPUMesh> mMesh;
    Matrix4 mMeshInstanceMatrix = Matrix4::smIdentity;
    WeakPtr<GPUShader> mShader;
    std::unordered_set<ClassId> mRenderPassIDs;
};

class GPURenderItem
{
public:
    void init(const GPURenderItemData& data, bool isStatic);
    void terminate();
    bool isStatic() const { return mIsStatic; } 

private:
    GPURenderItemData mGPURenderItemData;
    Slot mInstanceSlot;
    Slot mRenderSlot;
    Matrix4 mRendererModelMatrix;
    WeakPtr<GPUShaderPropertiesInstance> mGPUShaderPropertiesInstance;
    bool mIsStatic = false;

public:
    CRGET_SET(RendererModelMatrix)
    CRGET(GPURenderItemData)
    RGET(GPUShaderPropertiesInstance)
    CRGET_SET(RenderSlot)
    CRGET_SET(InstanceSlot)
    SET(IsStatic)
};
REGISTER_CLASS(GPURenderItem);
