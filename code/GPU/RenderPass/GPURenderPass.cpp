#include "GPU/RenderPass/GPURenderPass.h"
#include "GPU/Image/GPUImageUtils.hpp"
#include "GPU/Light/GPULight.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "vulkan/vulkan_core.h"

GPURenderPass::GPURenderPass(){}

void GPURenderPass::init(GPUContext* gpuContext, Core::WeakPtr<GPUInstanceRendererManager> gpuInstanceRendererManager, Core::WeakPtr<GPUUniformBuffersContainer> globalGPUUniformBuffersContainer, const GPURenderPassData& gpuRenderPassData,
    GPUSkeletalAnimationManager* gpuSkeletalAnimationManager, GPUShaderManager* gpuShaderManager)
{
    PROFILER_CPU()
    mGPUContext = gpuContext;
    mGPUSkeletalAnimationManager = gpuSkeletalAnimationManager;
    mGPUShaderManager = gpuShaderManager;
    mGPUInstanceRendererManager = gpuInstanceRendererManager;
    mGlobalGPUUniformBuffersContainer = globalGPUUniformBuffersContainer;
    mGPURenderPassData = gpuRenderPassData;

    mGPUUniformBuffersContainer.addUniformBuffer(mGPUContext, GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
}

void GPURenderPass::terminate()
{
    PROFILER_CPU()

    FOR_MAP(it, mGPUShaderPipelines)
    {
        it->second->terminate();
    }

    mGPUUniformBuffersContainer.terminate();
}
void GPURenderPass::begin(const GPURenderPassOutputData& gpuRenderPassOutputData)
{
    PROFILER_CPU()
    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];

    // Define the color attachment dynamically
    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = gpuRenderPassOutputData.mColorGPUImage->getVkImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = (VkAttachmentLoadOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentLoadOp;
    colorAttachment.storeOp = (VkAttachmentStoreOp) mGPURenderPassData.mColorAttachment.mGPUAttachmentStoreOp;
    colorAttachment.clearValue.color = {{0.1f, 0.2f, 0.1f, 1.0f}};
    
    colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.resolveImageView = VK_NULL_HANDLE;
    colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;

    if (mGPURenderPassData.mIsResolvePass)
    {
        VkImageView swapchainImageView = mGPUContext->vulkanSwapChain->getImageViews()[gpuRenderPassOutputData.mResolveSwapchainImageIndex];
        colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colorAttachment.resolveImageView = swapchainImageView;
        colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;

        // If resolving, we often don't need to store the MSAA data, saving bandwidth
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
    
    // Define the rendering scope
    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea = {{0, 0}, mGPUContext->vulkanSwapChain->getExtent()};
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    if(gpuRenderPassOutputData.mDepthGPUImage)
    {
        // Depth Attachment Setup
        VkRenderingAttachmentInfo depthAttachment{};
        depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depthAttachment.imageView = gpuRenderPassOutputData.mDepthGPUImage->getVkImageView(); 
        depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthAttachment.clearValue.depthStencil = {1.0f, 0};

        renderingInfo.pDepthAttachment = &depthAttachment;
    }

    vkCmdBeginRendering(vulkanCommandBuffer.getVkCommandBuffer(), &renderingInfo);

    //set dynamic viewport and scissor
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
    viewport.y = (float) mGPUContext->vulkanSwapChain->getExtent().height;
    viewport.width = (float) mGPUContext->vulkanSwapChain->getExtent().width;
    viewport.height = -(float) mGPUContext->vulkanSwapChain->getExtent().height;
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	vkCmdSetViewport(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor = {};
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent.width = mGPUContext->vulkanSwapChain->getExtent().width;
	scissor.extent.height = mGPUContext->vulkanSwapChain->getExtent().height;

	vkCmdSetScissor(vulkanCommandBuffer.getVkCommandBuffer(), 0, 1, &scissor);
}

void GPURenderPass::end()
{
    PROFILER_CPU()

    const GPUCommandBuffer& vulkanCommandBuffer = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame];
    vkCmdEndRendering(vulkanCommandBuffer.getVkCommandBuffer());
    // vkCmdEndRenderPass(vulkanCommandBuffer.getVkCommandBuffer());
}

void GPURenderPass::onResize()
{

}

void GPURenderPass::compileShader(const GPUInstanceRendererData& gpuInstanceRendererData)
{
    PROFILER_CPU_NAMED(RenderPass_compileShader)

    std::vector<GPUUniformBuffer> uniformBuffers;
    uniformBuffers.push_back(mGPUShaderManager->getGPUShaderPropertiesGPUUniformBuffer(gpuInstanceRendererData.mShader));

    Core::WeakPtr<GPUSkeletonState> skeletonState = mGPUSkeletalAnimationManager->getSkeletonStateFromMesh(gpuInstanceRendererData.mMesh);
    if(skeletonState)
    {
        uniformBuffers.push_back(mGPUSkeletalAnimationManager->getSkeletonRenderStateGPUUniformBuffer(skeletonState));
    }

    uniformBuffers.push_back(mGPUUniformBuffersContainer.getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData));
    uniformBuffers.push_back(mGlobalGPUUniformBuffersContainer->getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices));
    
    if(mGlobalGPUUniformBuffersContainer->containsUniformBuffer(GPULightBuiltIn::mLightsBufferData))
    {
        uniformBuffers.push_back(mGlobalGPUUniformBuffersContainer->getUniformBuffer(GPULightBuiltIn::mLightsBufferData));
    }  
    if(mGlobalGPUUniformBuffersContainer->containsUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData))
    {
        uniformBuffers.push_back(mGlobalGPUUniformBuffersContainer->getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData));
    }

    Core::WeakPtr<GPUInstanceRenderer> gpuInstanceRenderer = mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData);
    GPUShaderPipelineDepthStencilData gpuGPUShaderPipelineDepthStencilData;
    gpuGPUShaderPipelineDepthStencilData.mDepthTestEnable = VK_TRUE; //bool
    gpuGPUShaderPipelineDepthStencilData.mDepthWriteEnable = VK_TRUE; //bool
    gpuGPUShaderPipelineDepthStencilData.mDepthCompareOp = VK_COMPARE_OP_LESS; //VkCompareOp
    gpuGPUShaderPipelineDepthStencilData.mDepthBoundsTestEnable = VK_FALSE; //bool
    gpuGPUShaderPipelineDepthStencilData.mStencilTestEnable = gpuInstanceRendererData.mGPUShaderStencilData.mUseStencil; //bool
    
    VkStencilOpState vkStencilOpState;
    vkStencilOpState.failOp = (VkStencilOp) gpuInstanceRendererData.mGPUShaderStencilData.mStencilFailOp;
    vkStencilOpState.passOp = (VkStencilOp) gpuInstanceRendererData.mGPUShaderStencilData.mStencilPassOp;
    vkStencilOpState.depthFailOp = (VkStencilOp) gpuInstanceRendererData.mGPUShaderStencilData.mDepthFailOp;
    vkStencilOpState.compareOp = (VkCompareOp) gpuInstanceRendererData.mGPUShaderStencilData.mStencilFunction;
    vkStencilOpState.compareMask = 0xFF;
    vkStencilOpState.writeMask = 0xFF;
    vkStencilOpState.reference = 0;

    gpuGPUShaderPipelineDepthStencilData.mStencilFront = vkStencilOpState;
    gpuGPUShaderPipelineDepthStencilData.mStencilBack = vkStencilOpState;
    gpuGPUShaderPipelineDepthStencilData.mMinDepthBounds = 0; //float
    gpuGPUShaderPipelineDepthStencilData.mMaxDepthBounds = 0; //float
    GPUShaderCompilationData shaderCompilationData
    {
        Core::ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
        Core::HashedString(std::to_string(gpuInstanceRendererData.mShader->getID())),
        uniformBuffers,
        gpuInstanceRenderer->getGPUVertexBuffersContainer(),
        gpuGPUShaderPipelineDepthStencilData
    };

    if(!mGPUShaderPipelines.contains(gpuInstanceRendererData))
    {
        mGPUShaderPipelines.emplace(gpuInstanceRendererData, gpuInstanceRendererData.mShader->compileShader(shaderCompilationData));
    }
}

void GPURenderPass::addInstanceRendererData(const GPUInstanceRendererData& gpuInstanceRendererData)
{
    mGPUInstanceRendererRegistry.addInstanceRendererData(gpuInstanceRendererData);
    compileShader(gpuInstanceRendererData);
}

void GPURenderPass::preFramebufferEnabled()
{
}

void GPURenderPass::postFramebufferEnabled()
{
}

void GPURenderPass::preRender()
{
}

void GPURenderPass::postRender()
{
}

void GPURenderPass::render()
{
}

void GPURenderPass::renderGPUInstanceRenderer(const GPUInstanceRendererData& gpuInstanceRendererData)
{
    PROFILER_CPU()
    Core::WeakPtr<GPUInstanceRenderer> gpuInstanceRenderer = mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData);
    Core::WeakPtr<GPUShaderPipeline> gpuGPUShaderPipeline = mGPUShaderPipelines.at(gpuInstanceRendererData);
    
    auto& stencil = gpuInstanceRendererData.mGPUShaderStencilData;
    VkCommandBuffer cmd = mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame].getVkCommandBuffer();

    if (stencil.mUseStencil) {
        // Apply the dynamic states from the data object
        vkCmdSetStencilReference(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, stencil.mStencilValue);
        vkCmdSetStencilCompareMask(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, 0xFF);
        vkCmdSetStencilWriteMask(cmd, VK_STENCIL_FACE_FRONT_AND_BACK, 0xFF);

        mGPUContext->function_vkCmdSetStencilOpEXT(
            cmd, 
            VK_STENCIL_FACE_FRONT_AND_BACK, 
            (VkStencilOp)stencil.mStencilFailOp, 
            (VkStencilOp)stencil.mStencilPassOp, 
            (VkStencilOp)stencil.mDepthFailOp, 
            (VkCompareOp)stencil.mStencilFunction
        );
    }
    
    gpuGPUShaderPipeline->enable();
    gpuInstanceRenderer->render();
    gpuGPUShaderPipeline->disable();
}

void GPURenderPass::renderPass(const GPURenderPassOutputData& gpuRenderPassOutputData)
{
	PROFILER_CPU()

    updateGlobalData();

    // if(mRenderPassData.mOutputFramebufferData.isValid())
    // {
    //     preFramebufferEnabled();
    //     mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
    //     postFramebufferEnabled();
    // }

    // preRender();
    // render();
    // postRender();

    // if(mRenderPassData.mOutputFramebufferData.isValid())
    // {
    //     mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    // }
    begin(gpuRenderPassOutputData);
    {
        PROFILER_GPU_NAMED(renderPass, mGPUContext->mTracyContext, mGPUContext->vulkanCommandBuffers[mGPUContext->currentFrame].getVkCommandBuffer())
        render();
    }
    end();
}

void GPURenderPass::updateGlobalData()
{
}

Maths::Matrix4 GPURenderPass::calculateProjectionViewMatrix() const
{
	return Maths::Matrix4::smIdentity;
}