#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Graphics/Model/Model.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Material/MaterialManager.hpp"
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

    vulkanRenderPass = new GPURenderPass(GET_SYSTEM(GPUInstance).vulkanSwapChain, GET_SYSTEM(GPUInstance).vulkanDevice, GET_SYSTEM(GPUInstance).vulkanPhysicalDevice);
    vulkanGraphicsPipeline = new GPUGraphicsPipeline(vulkanRenderPass, GET_SYSTEM(GPUInstance).vulkanSwapChain, GET_SYSTEM(GPUInstance).vulkanDevice, GET_SYSTEM(GPUInstance).vulkanPhysicalDevice);

    if (!vulkanRenderPass->initialize())
    {
        CHECK_MSG(false, "Could not initialize Vulkan render pass");
    }
    // if (!vulkanGraphicsPipeline->initialize(*vertexShader, *fragmentShader, descriptorSetLayout))
    // {
    //     CHECK_MSG(false, "Could not initialize Vulkan graphics pipeline");
    // }

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

void RenderPass::terminate()
{
    vulkanGraphicsPipeline->terminate();
    vulkanRenderPass->terminate();

    VkAllocationCallbacks* allocationCallbacks = VK_NULL_HANDLE;
    vkDestroyImageView(GET_SYSTEM(GPUInstance).vulkanDevice->getDevice(), colorImageView, allocationCallbacks);
    vulkanColorImage->terminate();
    vkDestroyImageView(GET_SYSTEM(GPUInstance).vulkanDevice->getDevice(), depthImageView, allocationCallbacks);
    vulkanDepthImage->terminate();
    for (GPUFramebuffer framebuffer : framebuffers) {
        framebuffer.terminate();
    }
    framebuffers.clear();
    LOG("Destroyed Vulkan framebuffers");

    delete vulkanGraphicsPipeline;
    delete vulkanRenderPass;
    delete vulkanColorImage;
    delete vulkanDepthImage;
}

void RenderPass::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

	if (!mInstancedMeshRenderers.contains(instancedMeshData))
	{
        mInstancedMeshRenderers.insert(instancedMeshData);
      
        GPUVertexBuffersContainer gpuVertexBuffersContainer;
        instancedMeshData.mMesh->populateGPUVertexBuffersContainer(gpuVertexBuffersContainer, instancedMeshData.mIsStatic);

        Ptr<Shader> shader = getShader(instancedMeshData);
        shader->compileShader(
            ClassManager::getDynamicClassMetadata(this).mClassDefinition.mName,
            HashedString(std::to_string(instancedMeshData.mMaterial->getID())),
            gpuVertexBuffersContainer
        );

        setupShader(shader);
        bindShader(instancedMeshData);
    }
}

void RenderPass::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    InstancedMeshData instancedMeshData;
	instancedMeshData.init(renderer);

    Ptr<InstancedMeshRenderer> instancedMeshRenderer = mRenderPipeline->getInstancedMeshesMap().at(instancedMeshData);
    if(instancedMeshRenderer->isEmpty())
    {
        mInstancedMeshRenderers.erase(instancedMeshData);
    }
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
    Ptr<Shader> shader = getShader(instancedMeshData);
    shader->getGPUProgram()->bindUniformBuffer(GET_SYSTEM(MaterialManager).getMaterialPropertiesGPUUniformBuffer(instancedMeshData.mMaterial));
    
    Ptr<Model> model = GET_SYSTEM(ModelManager).getModelFromMesh(instancedMeshData.mMesh);
    if(model)
    {
        Ptr<GPUSkeletonState> skeletonState = model->getSkeletonState();
        if(skeletonState)
        {
            shader->getGPUProgram()->bindUniformBuffer(GET_SYSTEM(GPUSkeletalAnimationManager).getSkeletonRenderStateGPUUniformBuffer(skeletonState));
        }
    }

    shader->getGPUProgram()->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData));
    shader->getGPUProgram()->bindUniformBuffer(GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices));

    shader->bindTextures(shader->getGPUProgram(), GET_SYSTEM(MaterialManager).getMaterialTextureBindings(instancedMeshData.mMaterial));

    shader->getGPUProgram()->createDescriptors();
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
    Ptr<Shader> shader = getShader(instancedMeshData);


    shader->getGPUProgram()->enable();
    shader->enable();
    instancedMeshRenderer->render();
    shader->disable();
    shader->getGPUProgram()->disable();
}

void RenderPass::renderPass()
{
	PROFILER_CPU()

    updateGlobalData();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        preFramebufferEnabled();
        mOutputGPUFramebuffer.enable(GPUFramebufferOperationType::READ_AND_DRAW);
        postFramebufferEnabled();
    }

    preRender();
    render();
    postRender();

    if(mRenderPassData.mOutputFramebufferData.isValid())
    {
        mOutputGPUFramebuffer.disable(GPUFramebufferOperationType::READ_AND_DRAW);
    }
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

Ptr<Shader> RenderPass::getShader(const InstancedMeshData& instancedMeshData) const
{
    return instancedMeshData.mMaterial->getShader();
}

void RenderPass::setupShader(Ptr<Shader> shader) const
{
    PROFILER_CPU()
    FOR_ARRAY(i, mRenderPassData.mDependencies)
    {
        FramebufferBinding framebufferBinding
        {
            mRenderPassData.mDependencies[i].mSamplerName,
            mRenderPassData.mDependencies[i].mRenderPass->getOutputGPUFramebuffer().getAttachments().at(mRenderPassData.mDependencies[i].mAttachmentType).mAttachmentID,
            mRenderPassData.mDependencies[i].mStage
        };
        shader->addFramebufferBinding(framebufferBinding);
    }
}

bool RenderPass::initializeColorResources()
{
    VkFormat colorFormat = GET_SYSTEM(GPUInstance).vulkanSwapChain->getSurfaceFormat().format;

    GPUImage::Config colorImageConfig{};
    colorImageConfig.Width = GET_SYSTEM(GPUInstance).vulkanSwapChain->getExtent().width;
    colorImageConfig.Height = GET_SYSTEM(GPUInstance).vulkanSwapChain->getExtent().height;
    colorImageConfig.MipLevels = 1;
    colorImageConfig.SampleCount = GET_SYSTEM(GPUInstance).vulkanPhysicalDevice->getSampleCount();
    colorImageConfig.Format = colorFormat;
    colorImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    colorImageConfig.Usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    colorImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (!vulkanColorImage->initialize(colorImageConfig)) {
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
    depthImageConfig.Width = GET_SYSTEM(GPUInstance).vulkanSwapChain->getExtent().width;
    depthImageConfig.Height = GET_SYSTEM(GPUInstance).vulkanSwapChain->getExtent().height;
    depthImageConfig.Format = depthFormat;
    depthImageConfig.Tiling = VK_IMAGE_TILING_OPTIMAL;
    depthImageConfig.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    depthImageConfig.MemoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    depthImageConfig.MipLevels = 1;
    depthImageConfig.SampleCount = GET_SYSTEM(GPUInstance).vulkanPhysicalDevice->getSampleCount();

    if (!vulkanDepthImage->initialize(depthImageConfig)) {
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
    return GET_SYSTEM(GPUInstance).vulkanPhysicalDevice->findSupportedFormat(candidates, tiling, features);
}

bool RenderPass::initializeFramebuffers()
{
    for (VkImageView swapChainImageView : GET_SYSTEM(GPUInstance).vulkanSwapChain->getImageViews()) {
        GPUFramebuffer framebuffer;//(GET_SYSTEM(GPUInstance).vulkanDevice, GET_SYSTEM(GPUInstance).vulkanSwapChain, vulkanRenderPass);
        if (!framebuffer.initialize(colorImageView, depthImageView, swapChainImageView)) {
            CHECK_MSG(false,"Could not initialize framebuffers");
            return false;
        }
        framebuffers.push_back(framebuffer);
    }
    LOG("Created [{}] Vulkan framebuffers");
    return true;
}