#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "GPU/ShapeRenderer/GPUShapeRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Core/Window/WindowManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "GPU/Shader/BuiltIn/GPUShaderPBR.hpp"
#include "GPU/GPUInstance.hpp"
#include "GPU/Image/GPUImageUtils.hpp"

void RenderPipelinePBR::terminate()
{
    RenderPipeline::terminate();
    
    vulkanColorImage.terminate();
}

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

    VkFormat colorFormat = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSurfaceFormat().format;

    GPUImageData colorImageConfig{};
    colorImageConfig.Width = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().width;
    colorImageConfig.Height = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().height;
    colorImageConfig.MipLevels = 1;
    colorImageConfig.SampleCount = VK_SAMPLE_COUNT_1_BIT;
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_SAMPLED_BIT|/*VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    colorImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    vulkanColorImage.init(GET_SYSTEM(GPUInstance).mGPUContext, colorImageConfig);
    
    // colorImageView = GPUImageUtils::createImageView(gpuContext, vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageConfig.MipLevels);
    
    // if(!mFramebufferData.mIsResolveFramebuffer)
    {
        vulkanColorImage.transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
    GPURenderPassData renderPassGeometryData;
    renderPassGeometryData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::CLEAR;
    renderPassGeometryData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::STORE;
    renderPassGeometryData.mColorAttachment.mGPUImage = &vulkanColorImage;
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    // initRenderPass<RenderPassGeometry>(renderPassGeometryData);

    mGPURenderGraph.initRenderPass<RenderPassGeometry>(renderPassGeometryData);

    GPURenderPassData renderPassUIData;
    renderPassUIData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;
    renderPassUIData.mColorAttachment.mGPUAttachmentStoreOp = GPUAttachmentStoreOp::DONT_CARE;
    renderPassUIData.mColorAttachment.mGPUImage = &vulkanColorImage;

    renderPassUIData.mGeometricSpace = GeometricSpace::SCREEN;
    // initRenderPass<RenderPassUI>(renderPassUIData);

    mGPURenderGraph.initRenderPass<RenderPassUI>(renderPassUIData);

    GPURenderPassData renderPassResolveData;
    // renderPassResolveData.mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;
    renderPassResolveData.mIsResolvePass = true;
    renderPassResolveData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;

    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(GPUShaderManager).createShader<GPUShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    // initRenderPass<RenderPassBase>(renderPassResolveData);

    mGPURenderGraph.initRenderPass<RenderPassBase>(renderPassResolveData);
    

    // FOR_RANGE(i, 0, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImages().size())
    // {
    //     GPUImageUtils::transitionImageLayout(
    //         GET_SYSTEM(GPUInstance).mGPUContext,
    //         GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImages()[i],
    //         GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSurfaceFormat().format,
    //         VK_IMAGE_LAYOUT_UNDEFINED,
    //         VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    //         1);
    // }
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
        // {
            // WeakPtr<PointLight> pointLight = renderData.mPointLights.at(i);
            // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
            // renderPassShadowMap->mDirectionalLight = renderData.mDirectionalLight;
            // renderPassShadowMap->renderPass();
            WeakPtr<RenderPassGeometry> renderPassGeometry = mGPURenderGraph.getRenderPass<RenderPassGeometry>();
            renderPassGeometry->mDirectionalLight = renderData.mDirectionalLight;
            // renderPassGeometry->getGPURenderPass()->clearColor();
            renderPassGeometry->renderPass();
        // }
        // WeakPtr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
        // renderPassGeometry->mPointLight = renderData.mPointLights[0];
        // renderPassGeometry->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();

        // vulkanRenderPass->clearColor();
        // vulkanRenderPass->clearDepthStencil();

        WeakPtr<RenderPassUI> renderPassUI = mGPURenderGraph.getRenderPass<RenderPassUI>();
        renderPassUI->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();

        // FOR_RANGE(i, 0, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImageViews().size())
        // { 
            // VkFormat colorFormat = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSurfaceFormat().format;
            // GPUImageUtils::transitionImageLayout(GET_SYSTEM(GPUInstance).mGPUContext, renderPassGeometry->getGPURenderPass()->getOutputGPUFramebuffer().vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);
        // }

//         GPUImageUtils::copyImageToImage(GET_SYSTEM(GPUInstance).mGPUContext, renderPassGeometry->getGPURenderPass()->getOutputGPUFramebuffer().vulkanColorImage.getVkImage(),
//     VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex],
// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().width, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().height, 0,0);

        vulkanColorImage.copyToVkImage(GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED);
        GPUImageUtils::transitionImageLayout(GET_SYSTEM(GPUInstance).mGPUContext, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex], VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);

        WeakPtr<RenderPassBase> renderPassResolve = mGPURenderGraph.getRenderPass<RenderPassBase>();
        renderPassResolve->renderPass();
    }
    if (!vulkanCommandBuffer.end()) {
        CHECK_MSG(false, "Could not end frame");
    }

    GET_SYSTEM(GPUInstance).mGPUContext->commandSubmission();
    GET_SYSTEM(GPUInstance).mGPUContext->framePresentation({swapChainImageIndex});
}