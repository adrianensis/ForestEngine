#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/Material/Shader/Shader.hpp"
#include "Graphics/Model/ModelManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"
#include "Core/ECS/EntityHandler.hpp"

void RenderPass::init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData)
{
    mRenderPipeline = renderPipeline;
    mRenderPassData = renderPassData;

    if(mRenderPassData.mOutputFramebufferData.isValid())
    { 
        mOutputGPUFramebuffer.init(mRenderPassData.mOutputFramebufferData);
    }

    vulkanRenderPass = new GPURenderPass(GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain, GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice, GET_SYSTEM(GPUInstance).mGPUContext->vulkanPhysicalDevice);
}

void RenderPass::terminate()
{
    vulkanRenderPass->terminate();

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), colorImageView, allocationCallbacks);
    vulkanColorImage->terminate();
    vkDestroyImageView(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), depthImageView, allocationCallbacks);
    vulkanDepthImage->terminate();
    for (GPUFramebuffer framebuffer : framebuffers) {
        framebuffer.terminate();
    }
    framebuffers.clear();
    LOG("Destroyed Vulkan framebuffers");

    delete vulkanRenderPass;
    delete vulkanColorImage;
    delete vulkanDepthImage;
}

void RenderPass::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	// InstancedMeshData instancedMeshData;
	// instancedMeshData.init(renderer);

	if (!mGPUPrograms.contains(renderer->getMaterialInstance()->mMaterial->getID()))
	// if (!mInstancedMeshRenderers.contains(instancedMeshData))
	{
        // mInstancedMeshRenderers.insert(instancedMeshData);

        mGPUPrograms.emplace(renderer->getMaterialInstance()->mMaterial->getID(), renderer->getMaterialInstance()->mMaterial->getShader()->compileShader(vulkanRenderPass,
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            HashedString(std::to_string(renderer->getMaterialInstance()->mMaterial->getID()))
        ));

        // setupShader(mGPUPrograms.at(instancedMeshData));
        // bindShader(instancedMeshData);
    }
}

void RenderPass::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    // Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    // if(instancedMeshRenderer->isEmpty())
    // {
    //     mInstancedMeshRenderers.erase(instancedMeshData);
    // }
}

void RenderPass::preFramebufferEnabled()
{
}

void RenderPass::postFramebufferEnabled()
{
}

void RenderPass::bindShader(const InstancedMeshData& instancedMeshData)
{
    PROFILER_CPU()
    Ptr<GPUProgram> gpuProgram = mGPUPrograms.at(instancedMeshData.mMaterial->getID());
    gpuProgram->bindUniformBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUUniformBuffer(instancedMeshData.mMaterial));
    
    Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    if(model)
    {
        Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            gpuProgram->bindUniformBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
        }
    }

    gpuProgram->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData));
    gpuProgram->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices));

    // mGPUPrograms.at(instancedMeshData)->bindTextures(mGPUPrograms.at(instancedMeshData)->getGPUProgram(), GET_SYSTEM(MaterialManager).getMaterialTextureBindings(instancedMeshData.mMaterial));

    // shader->createDescriptors();
}

void RenderPass::preRender()
{
}

void RenderPass::postRender()
{
}

void RenderPass::render()
{
}

void RenderPass::renderBatch(const InstancedMeshData& instancedMeshData)
{
    PROFILER_CPU()
    Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    Ptr<GPUProgram> gpuProgram = mGPUPrograms.at(instancedMeshData.mMaterial->getID());

    gpuProgram->enable();
    instancedMeshRenderer->render();
    gpuProgram->disable();
}

void RenderPass::renderPass()
{
	PROFILER_CPU()

    // updateGlobalData();

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

    frameAcquisition();
    commandRecording();
    commandSubmission();
    framePresentation();
}

void RenderPass::updateGlobalData()
{
	PROFILER_CPU()

    Matrix4 ortho;
    ortho.ortho(-1, 1, -1, 1, -1000, 1000);

    TypedComponentHandler<Camera> camera = GET_SYSTEM(CameraManager).getCamera();

    Matrix4 projectionViewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mProjectionMatrix : ortho;
    Matrix4 viewMatrix = mRenderPassData.mGeometricSpace == GeometricSpace::WORLD ? camera->mViewMatrix : Matrix4::smIdentity;

    projectionViewMatrix.mul(viewMatrix);

    GPUBuiltIn::UniformBuffers::GPUGlobalData gpuGlobalData =
    {
        projectionViewMatrix,
        camera->getOwnerEntity()->getFirstComponent<Transform>()->getWorldPosition()
    };
	GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData).setData(gpuGlobalData);
}

void RenderPass::setupShader(Ptr<Shader> shader) const
{
    PROFILER_CPU()
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        FramebufferBinding framebufferBinding
        {
            // mRenderPassData.mDependencies[i].mSamplerName,
            // mRenderPassData.mDependencies[i].mRenderPass->getOutputGPUFramebuffer().getAttachments().at(mRenderPassData.mDependencies[i].mAttachmentType).mAttachmentID,
            // mRenderPassData.mDependencies[i].mStage
        };
        // shader->addFramebufferBinding(framebufferBinding);
    }
}

void RenderPass::compile()
{
    // mGPUPrograms.at(instancedMeshData)->createDescriptors();

    if (!vulkanRenderPass->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan render pass");
    }

    if (!initializeColorResources())
    {
        CHECK_MSG(false, "Could not initialize color resources");
    }
    if (!initializeDepthResources())
    {
        CHECK_MSG(false, "Could not initialize depth resources");
    }
    if (!initializeFramebuffers())
    {
        CHECK_MSG(false, "Could not initialize Vulkan framebuffers");
    }
}

bool RenderPass::initializeColorResources()
{
    VkFormat colorFormat = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSurfaceFormat().format;

    GPUImage::Config colorImageConfig{};
    colorImageConfig.Width = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().width;
    colorImageConfig.Height = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().height;
    colorImageConfig.MipLevels = 1;
    colorImageConfig.SampleCount = GET_SYSTEM(GPUInstance).mGPUContext->vulkanPhysicalDevice->getSampleCount();
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    vulkanColorImage = new GPUImage();
    if (!vulkanColorImage->init(GET_SYSTEM(GPUInstance).mGPUContext, colorImageConfig)) {
        CHECK_MSG(false,"Could not initialize color image");
        return false;
    }
    colorImageView = GET_SYSTEM(GPUInstance).createImageView(vulkanColorImage->getVkImage(), colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageConfig.MipLevels);
    return true;
}

bool RenderPass::initializeDepthResources()
{
    VkFormat depthFormat = findDepthFormat();

    GPUImage::Config depthImageConfig{};
    depthImageConfig.Width = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().width;
    depthImageConfig.Height = GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getExtent().height;
    depthImageConfig.Format = depthFormat;
    depthImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageConfig.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthImageConfig.MipLevels = 1;
    depthImageConfig.SampleCount = GET_SYSTEM(GPUInstance).mGPUContext->vulkanPhysicalDevice->getSampleCount();

    vulkanDepthImage = new GPUImage();
    if (!vulkanDepthImage->init(GET_SYSTEM(GPUInstance).mGPUContext, depthImageConfig)) {
        CHECK_MSG(false,"Could not initialize depth image");
        return false;
    }
    depthImageView = GET_SYSTEM(GPUInstance).createImageView(vulkanDepthImage->getVkImage(), depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageConfig.MipLevels);
    GET_SYSTEM(GPUInstance).transitionImageLayout(vulkanDepthImage->getVkImage(), depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depthImageConfig.MipLevels);
    return true;
}

VkFormat RenderPass::findDepthFormat()
{
    std::vector<VkFormat> candidates = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
    };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    return GET_SYSTEM(GPUInstance).mGPUContext->vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}

bool RenderPass::initializeFramebuffers()
{
    for (VkImageView swapChainImageView : GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getImageViews()) {
        GPUFramebuffer framebuffer;//(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice, GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain, vulkanRenderPass);
        if (!framebuffer.initialize(GET_SYSTEM(GPUInstance).mGPUContext, vulkanRenderPass, colorImageView, depthImageView, swapChainImageView)) {
            CHECK_MSG(false,"Could not initialize framebuffers");
            return false;
        }
        framebuffers.push_back(framebuffer);
    }
    LOG("Created [{}] Vulkan framebuffers");
    return true;
}


void RenderPass::frameAcquisition()
{
    // Wait until the previous frame has finished
    constexpr uint32_t fenceCount = 1;
    constexpr VkBool32 waitForAllFences = VK_TRUE;
    constexpr uint64_t waitForFenceTimeout = UINT64_MAX;
    VkFence inFlightFence = GET_SYSTEM(GPUInstance).mGPUContext->inFlightFences[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    vkWaitForFences(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), fenceCount, &inFlightFence, waitForAllFences, waitForFenceTimeout);

    // Acquire an image from the swap chain
    VkFence acquireNextImageFence = VK_NULL_HANDLE;
    constexpr uint64_t acquireNextImageTimeout = UINT64_MAX;
    VkSemaphore imageAvailableSemaphore = GET_SYSTEM(GPUInstance).mGPUContext->imageAvailableSemaphores[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    VkResult acquireNextImageResult = vkAcquireNextImageKHR(
            GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(),
            GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSwapChain(),
            acquireNextImageTimeout,
            imageAvailableSemaphore,
            acquireNextImageFence,
            &swapChainImageIndex
    );
    // VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a gpuWindow resize.
    if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
        //recreateRenderingObjects();
        CHECK_MSG(false, "TODO: implement gpuWindow resize");
        return;
    }
    // VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
    if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR) {
        CHECK_MSG(false, "Could not acquire swap chain image");
    }

    // After waiting, we need to manually reset the fence to the unsignaled state
    vkResetFences(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), fenceCount, &inFlightFence);
}

void RenderPass::commandRecording()
{
    const GPUCommandBuffer& vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    vulkanCommandBuffer.reset();
    vulkanCommandBuffer.begin();

    vulkanRenderPass->begin(vulkanCommandBuffer, framebuffers.at(swapChainImageIndex));

    // vulkanGraphicsPipeline->bind(vulkanCommandBuffer);

    // // VkBuffer vertexBuffers[] = {vulkanVertexBuffer->getGPUBuffer().getVkBuffer()};
    // // VkDeviceSize vertexBufferOffsets[] = {0};
    // // constexpr uint32_t firstBinding = 0;
    // // constexpr uint32_t bindingCount = 1;
    // // vkCmdBindVertexBuffers(vulkanCommandBuffer.getVkCommandBuffer(), firstBinding, bindingCount, vertexBuffers, vertexBufferOffsets);

    // // constexpr VkDeviceSize indexBufferOffset = 0;
    // // constexpr VkIndexType indexType = VK_INDEX_TYPE_UINT32;
    // // vkCmdBindIndexBuffer(vulkanCommandBuffer.getVkCommandBuffer(), vulkanIndexBuffer->getGPUBuffer().getVkBuffer(), indexBufferOffset, indexType);

    // // VkDescriptorSet descriptorSet = descriptorSets[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    // // VkPipelineBindPoint pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // // VkPipelineLayout pipelineLayout = vulkanGraphicsPipeline->getPipelineLayout();
    // // constexpr uint32_t firstSet = 0;
    // // constexpr uint32_t descriptorSetCount = 1;
    // // constexpr uint32_t dynamicOffsetCount = 0;
    // // constexpr uint32_t* dynamicOffsets = nullptr;
    // // vkCmdBindDescriptorSets(vulkanCommandBuffer.getVkCommandBuffer(), pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount, &descriptorSet, dynamicOffsetCount, dynamicOffsets);

    // // constexpr uint32_t instanceCount = 1;
    // // constexpr uint32_t firstVertex = 0;
    // // constexpr int32_t vertexOffset = 0;
    // // constexpr uint32_t firstInstance = 0;
    // // vkCmdDrawIndexed(vulkanCommandBuffer.getVkCommandBuffer(), (uint32_t) indices.size(), instanceCount, firstVertex, vertexOffset, firstInstance);

    vulkanRenderPass->end(vulkanCommandBuffer);

    if (!vulkanCommandBuffer.end()) {
        CHECK_MSG(false, "Could not end frame");
    }
}

void RenderPass::commandSubmission()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const GPUCommandBuffer& vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->vulkanCommandBuffers[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    VkCommandBuffer vkCommandBuffer = vulkanCommandBuffer.getVkCommandBuffer();
    submitInfo.pCommandBuffers = &vkCommandBuffer;
    submitInfo.commandBufferCount = 1;

    // Wait with writing colors to the image until it's available
    VkSemaphore imageAvailableSemaphore = GET_SYSTEM(GPUInstance).mGPUContext->imageAvailableSemaphores[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.waitSemaphoreCount = 1;

    // Which semaphores to signal once the command buffer(s) have finished execution
    VkSemaphore renderFinishedSemaphore = GET_SYSTEM(GPUInstance).mGPUContext->renderFinishedSemaphores[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.signalSemaphoreCount = 1;

    // Submit recorded graphics commands
    constexpr uint32_t submitCount = 1;
    VkFence inFlightFence = GET_SYSTEM(GPUInstance).mGPUContext->inFlightFences[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    if (vkQueueSubmit(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getGraphicsQueue(), submitCount, &submitInfo, inFlightFence) != VK_SUCCESS) {
        //VD_LOG_CRITICAL("Could not submit to graphics queue");
        throw std::runtime_error("Could not submit to graphics queue");
    }
}

void RenderPass::framePresentation()
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    // Which semaphores to wait on before presentation can happen
    VkSemaphore renderFinishedSemaphore = GET_SYSTEM(GPUInstance).mGPUContext->renderFinishedSemaphores[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.waitSemaphoreCount = 1;

    // Which swap chain to present image to
    VkSwapchainKHR swapChains[] = {GET_SYSTEM(GPUInstance).mGPUContext->vulkanSwapChain->getSwapChain()};
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &swapChainImageIndex;
    presentInfo.swapchainCount = 1;

    // Present image to swap chain
    VkResult presentResult = vkQueuePresentKHR(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getPresentQueue(), &presentInfo);
    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR /*|| windowResized*/) {
        // windowResized = false;
        // recreateRenderingObjects();
    } else if (presentResult != VK_SUCCESS) {
        //VD_LOG_CRITICAL("Could not present image to swap chain");
        throw std::runtime_error("Could not present image to swap chain");
    }
}
