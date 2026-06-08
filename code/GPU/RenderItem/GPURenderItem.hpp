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
    void setRendererModelMatrix(Maths::Matrix4 modelMatrix) { mRendererModelMatrix = modelMatrix; }
    const Maths::Matrix4& getRendererModelMatrix() const { return mRendererModelMatrix; }

    const auto& getGPURenderItemData() const { return mGPURenderItemData; }
    auto& getGPUShaderPropertiesInstance() const { return mGPUShaderPropertiesInstance; }
    GPU::u32 getRenderSlot() const { return mRenderSlot; }
    GPU::u32 getInstanceSlot() const { return mInstanceSlot; }
    void setRenderSlot(GPU::u32 renderSlot) { mRenderSlot = renderSlot; }
    void setInstanceSlot(GPU::u32 instanceSlot) { mInstanceSlot = instanceSlot; }
    void setIsStatic(bool isStatic) { mIsStatic = isStatic; }
    void setUpdateMatrix(bool updateMatrix) { mUpdateMatrix = updateMatrix; }
    bool getUpdateMatrix() const { return mUpdateMatrix; }
};

