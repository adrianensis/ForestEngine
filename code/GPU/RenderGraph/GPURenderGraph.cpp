#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "GPU/Image/GPUImageUtils.hpp"

void GPURenderGraph::init(Core::Ptr<GPUContext> gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPUInstanceRendererManager = gpuInstanceRendererManager;

    VkFormat colorFormat = GPUInstance::getInstance().mGPUContext->vulkanSwapChain->getSurfaceFormat().format;

    GPUImageData colorImageConfig{};
    colorImageConfig.Width = GPUInstance::getInstance().mGPUContext->vulkanSwapChain->getExtent().width;
    colorImageConfig.Height = GPUInstance::getInstance().mGPUContext->vulkanSwapChain->getExtent().height;
    colorImageConfig.MipLevels = 1;
    colorImageConfig.SampleCount = VK_SAMPLE_COUNT_1_BIT;
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_SAMPLED_BIT|/*VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT /*| VK_IMAGE_USAGE_TRANSFER_DST_BIT*/;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    colorImageConfig.InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    vulkanColorImage.init(GPUInstance::getInstance().mGPUContext, colorImageConfig);
    
    // colorImageView = GPUImageUtils::createImageView(gpuContext, vulkanColorImage.getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageConfig.MipLevels);
    
    // if(!mFramebufferData.mIsResolveFramebuffer)
    {
        vulkanColorImage.transition(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    GPURenderPassData renderPassResolveData;
    renderPassResolveData.mIsResolvePass = true;
    renderPassResolveData.mColorAttachment.mGPUAttachmentLoadOp = GPUAttachmentLoadOp::LOAD;

    GPURenderPassOutputData renderPassOutputData;
    renderPassOutputData.mColorGPUImage = &vulkanColorImage;
    
    mRenderPassResolve = Core::OwnerPtr<GPURenderPass>::newObject();
    mRenderPassResolve->init(mGPUContext, mGPUInstanceRendererManager, renderPassResolveData, renderPassOutputData);
}

void GPURenderGraph::render(GPURenderGraphData& renderData)
{
    Core::u32 swapChainImageIndex = mGPUContext->frameAcquisition();
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vulkanCommandBuffer.reset();
    vulkanCommandBuffer.begin();

    {
        PROFILER_GPU_NAMED(renderPass, mGPUContext->mTracyContext, mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame].getVkCommandBuffer())

        FOR_ARRAY(i, mRenderPassesArray)
        {
            Core::WeakPtr<GPURenderPass> renderPass = mRenderPassesArray[i];
            renderPass->renderPass();
        }

        vulkanColorImage.copyToVkImage(mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED);
        GPUImageUtils::transitionImageLayout(mGPUContext, mGPUContext->vulkanSwapChain->getImages()[swapChainImageIndex], VK_FORMAT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 1);

        mRenderPassResolve->renderPass();
    }
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
    vulkanColorImage.terminate();

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
            Core::Ptr<GPURenderPass> renderPass = mRenderPassMap.at(*it);
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

void GPURenderGraph::updateLights(GPURenderGraphData& renderData)
{
	PROFILER_CPU()

    // GPULightBuiltIn::LightsData lightsData;
    // FOR_ARRAY(i, renderData.mPointLights)
    // {
    //     lightsData.mPointLights[i] = renderData.mPointLights[i]->calculateLightData();
    // }

    // if(renderData.mDirectionalLight)
    // {
    //     lightsData.mDirectionalLight = renderData.mDirectionalLight->calculateLightData();
    // }

    // getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).setData(lightsData);
}