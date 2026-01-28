#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "GPU/Image/GPUImageUtils.hpp"
#include "vulkan/vulkan_core.h"

void GPURenderGraph::init(GPUContext* gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, Core::WeakPtr<GPUUniformBuffersContainer> globalGPUUniformBuffersContainer,
    GPUSkeletalAnimationManager* gpuSkeletalAnimationManager, GPUShaderManager* gpuShaderManager)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPUInstanceRendererManager = gpuInstanceRendererManager;
    mGlobalGPUUniformBuffersContainer = globalGPUUniformBuffersContainer;

    VkFormat colorFormat = mGPUContext->vulkanSwapChain->getSurfaceFormat().format;

    GPUImageData colorBufferImageConfig{};
    colorBufferImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    colorBufferImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    colorBufferImageConfig.MipLevels = 1;
    colorBufferImageConfig.SampleCount = VK_SAMPLE_COUNT_4_BIT;
    colorBufferImageConfig.Format = colorFormat;
    colorBufferImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorBufferImageConfig.Usage = VK_IMAGE_USAGE_SAMPLED_BIT|/*VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    colorBufferImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    colorBufferImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    mColorBufferImage.init(mGPUContext, colorBufferImageConfig);

    GPUImageData depthBufferImageConfig{};
    depthBufferImageConfig.Width = mGPUContext->vulkanSwapChain->getExtent().width;
    depthBufferImageConfig.Height = mGPUContext->vulkanSwapChain->getExtent().height;
    depthBufferImageConfig.MipLevels = 1;
    depthBufferImageConfig.SampleCount = VK_SAMPLE_COUNT_4_BIT;
    depthBufferImageConfig.Format = GPUImageUtils::findDepthFormat(mGPUContext);;
    depthBufferImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthBufferImageConfig.Usage = VK_IMAGE_USAGE_SAMPLED_BIT|/*VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |*/ VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    depthBufferImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthBufferImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    mDepthBufferImage.init(mGPUContext, depthBufferImageConfig);

    mColorBufferImage.transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    mDepthBufferImage.transition(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    GPURenderPassData renderPassResolveData;
    renderPassResolveData.mIsResolvePass = true;
    renderPassResolveData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;

    mRenderPassResolve = Core::OwnerPtr<GPURenderPass>::newObject();
    mRenderPassResolve->init(mGPUContext, mGPUInstanceRendererManager, mGlobalGPUUniformBuffersContainer, renderPassResolveData, gpuSkeletalAnimationManager, gpuShaderManager);
}

void GPURenderGraph::render()
{
    Core::u32 swapChainImageIndex = mGPUContext->frameAcquisition();
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];

    vulkanCommandBuffer.reset();
    vulkanCommandBuffer.begin();
    
    VkImage swapchainImage = mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex];

    GPUImageUtils::transitionImageLayout(mGPUContext, swapchainImage, 
				                      mGPUContext->vulkanSwapChain->getSurfaceFormat().format,
				                      VK_IMAGE_LAYOUT_UNDEFINED, 
				                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);

    {
        PROFILER_GPU_NAMED(renderPass, mGPUContext->mTracyContext, mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame].getVkCommandBuffer())

        FOR_ARRAY(i, mRenderPassesArray)
        {
            GPURenderPassOutputData renderPassOutputData;
            renderPassOutputData.mColorGPUImage = &mColorBufferImage;
            renderPassOutputData.mDepthGPUImage = &mDepthBufferImage;

            Core::WeakPtr<GPURenderPass> renderPass = mRenderPassesArray[i];
            renderPass->renderPass(renderPassOutputData);
        }

        GPURenderPassOutputData renderPassOutputData;
        renderPassOutputData.mResolveSwapchainImageIndex = swapChainImageIndex;
        renderPassOutputData.mColorGPUImage = &mColorBufferImage;
        renderPassOutputData.mDepthGPUImage = &mDepthBufferImage;

        mRenderPassResolve->renderPass(renderPassOutputData);
    }

    GPUImageUtils::transitionImageLayout(mGPUContext, swapchainImage, 
				                      mGPUContext->vulkanSwapChain->getSurfaceFormat().format,
				                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
				                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);
    if (!vulkanCommandBuffer.end()) {
        CHECK_MSG(false, "Could not end frame");
    }

    mGPUContext->commandSubmission();
    mGPUContext->framePresentation({swapChainImageIndex});
}

void GPURenderGraph::update()
{
	PROFILER_CPU()

}

void GPURenderGraph::terminate()
{
    mColorBufferImage.terminate();
    mDepthBufferImage.terminate();

    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}

    mRenderPassResolve->terminate();
}

void GPURenderGraph::onResize()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->onResize();
	}
}

void GPURenderGraph::addRenderer(Core::WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderItem);

    FOR_LIST(it, renderItem->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            Core::WeakPtr<GPURenderPass> renderPass = mRenderPassMap.at(*it);
            renderPass->addInstanceRendererData(gpuInstanceRendererData);
        }
    }
}

void GPURenderGraph::removeRenderer(Core::WeakPtr<GPURenderItem> renderItem)
{
    PROFILER_CPU()
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderItem);

    FOR_LIST(it, renderItem->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            // TODO: Only remove if renderers count == 0
            // renderPass->getGPUInstanceRendererRegistry().removeInstanceRendererData(gpuInstanceRendererData);
        }
    }
}