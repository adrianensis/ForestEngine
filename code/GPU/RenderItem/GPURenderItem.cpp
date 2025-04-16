#include "GPU/RenderItem/GPURenderItem.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"

void GPURenderItem::init(const GPURenderItemData& data, bool isStatic) 
{
    mGPURenderItemData = data;
    mIsStatic = isStatic;
    mGPUShaderPropertiesInstance = GET_SYSTEM(GPUShaderManager).createGPUShaderPropertiesInstance(mGPURenderItemData.mShader);
    mGPUShaderPropertiesInstance->setDirty();
}

void GPURenderItem::terminate() 
{
    GET_SYSTEM(GPUShaderManager).freeGPUShaderPropertiesInstance(mGPUShaderPropertiesInstance);
    mRenderSlot.reset();
    mInstanceSlot.reset();
}