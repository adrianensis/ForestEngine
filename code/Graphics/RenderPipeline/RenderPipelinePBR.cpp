#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Graphics/Renderer/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Core/Window/WindowManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderPBR.hpp"
#include "GPU/GPUInstance.hpp"

void RenderPipelinePBR::compile()
{
    PROFILER_CPU()

    // RenderPassData renderPassShadowMapData;
    // renderPassShadowMapData.mOutputFramebufferData.set(
    //     {
    //         GPUFramebufferAttachmentType::DEPTH
    //     },
    //     2048, 2048
    // );
    // initRenderPass<RenderPassShadowMap>(renderPassShadowMapData);

    // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
    RenderPassData renderPassGeometryData;
    renderPassGeometryData.mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::CLEAR;
    renderPassGeometryData.mGPURenderPassData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::STORE;
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    initRenderPass<RenderPassGeometry>(renderPassGeometryData);

    // RenderPassData renderPassResolveData;
    // renderPassResolveData.mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;
    // renderPassResolveData.mGPURenderPassData.mIsResolvePass = true;
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    // initRenderPass<RenderPass>(renderPassResolveData);
    
    // RenderPassData renderPassUIData;
    // renderPassUIData.mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;
    // renderPassUIData.mGPURenderPassData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::DONT_CARE;
    // renderPassUIData.mGeometricSpace = GeometricSpace::SCREEN;
    // initRenderPass<RenderPassUI>(renderPassUIData);
}


void RenderPipelinePBR::render(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    u32 swapChainImageIndex = GET_SYSTEM(GPUInstance).mGPUContext->frameAcquisition();
    const GPUCommandBuffer& vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    vulkanCommandBuffer.reset();
    vulkanCommandBuffer.begin();

    {
        PROFILER_GPU_NAMED(renderPass, GET_SYSTEM(GPUInstance).mGPUContext->mTracyContext, GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame].getVkCommandBuffer())
    //     // updateLights(renderData);

    //	GET_SYSTEM(GPUInterface).clear();

        // FOR_ARRAY(i, renderData.mPointLights)
        {
            // WeakPtr<PointLight> pointLight = renderData.mPointLights.at(i);
            // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
            // renderPassShadowMap->mDirectionalLight = renderData.mDirectionalLight;
            // renderPassShadowMap->renderPass();
            WeakPtr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
            renderPassGeometry->mDirectionalLight = renderData.mDirectionalLight;
            // renderPassGeometry->getGPURenderPass()->clearColor();
            renderPassGeometry->renderPass();
        }
        // WeakPtr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
        // renderPassGeometry->mPointLight = renderData.mPointLights[0];
        // renderPassGeometry->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();

        // vulkanRenderPass->clearColor();
        // vulkanRenderPass->clearDepthStencil();

        // WeakPtr<RenderPassUI> renderPassUI = getRenderPass<RenderPassUI>();
        // renderPassUI->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();

        // WeakPtr<RenderPass> renderPassResolve = getRenderPass<RenderPass>();
        // renderPassResolve->renderPass();
    }
    if (!vulkanCommandBuffer.end()) {
        CHECK_MSG(false, "Could not end frame");
    }

    GET_SYSTEM(GPUInstance).mGPUContext->commandSubmission();
    GET_SYSTEM(GPUInstance).mGPUContext->framePresentation({swapChainImageIndex});
}