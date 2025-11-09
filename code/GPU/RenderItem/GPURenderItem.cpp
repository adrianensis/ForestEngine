#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"

void GPURenderItem::init(const GPURenderItemData& data, bool isStatic, Core::Ptr<GPUShaderManager> gpuShaderManager) 
{
    mGPURenderItemData = data;
    mIsStatic = isStatic;
    mGPUShaderPropertiesInstance = gpuShaderManager->createGPUShaderPropertiesInstance(mGPURenderItemData.mShader);
    mGPUShaderPropertiesInstance->setDirty();
}

void GPURenderItem::terminate(Core::Ptr<GPUShaderManager> gpuShaderManager) 
{
    gpuShaderManager->freeGPUShaderPropertiesInstance(mGPUShaderPropertiesInstance);
    mRenderSlot.reset();
    mInstanceSlot.reset();
}