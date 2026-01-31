#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPipelinePBR::compile()
{
    PROFILER_CPU()

    GPURenderPassData renderPassGeometryData;
    renderPassGeometryData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::CLEAR;
    renderPassGeometryData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::STORE;
    mGPURenderGraph.initRenderPass<RenderPassGeometry>(renderPassGeometryData, 
        GPUInstance::getInstance().mGPUSkeletalAnimationManager, 
        GPUInstance::getInstance().mGPUShaderManager, 
        GPUInstance::getInstance().mGPUDescriptorManager);

    GPURenderPassData renderPassUIData;
    renderPassUIData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;
    renderPassUIData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::DONT_CARE;
    renderPassUIData.mGeometricSpace = Maths::GeometricSpace::SCREEN;

    mGPURenderGraph.initRenderPass<RenderPassUI>(renderPassUIData, 
        GPUInstance::getInstance().mGPUSkeletalAnimationManager, 
        GPUInstance::getInstance().mGPUShaderManager, 
        GPUInstance::getInstance().mGPUDescriptorManager);
}