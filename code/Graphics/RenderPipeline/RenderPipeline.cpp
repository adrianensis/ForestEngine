#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Material/MaterialManager.hpp"
#include "Graphics/GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void RenderPipeline::init()
{
    LOG_TRACE()
    PROFILER_CPU()
    initBuffers();
}

void RenderPipeline::update()
{
	PROFILER_CPU()
    PROFILER_BLOCK_CPU(updateRenderers);
    FOR_RANGE(i, *mUsedSlots.begin(), (*mUsedSlots.rbegin())+1)
    {
        TypedComponentHandler<MeshRenderer> renderer = mRenderers[i];
        if(renderer.isValid())
        {
            processRenderer(renderer);
        }
    }

    // std::for_each(
    // std::execution::par,
    // mRenderers.begin(),
    // mRenderers.end(),
    // [this](TypedComponentHandler<MeshRenderer> renderer)
    // {
    //     if(renderer.isValid())
    //     {
    //         processRenderer(renderer);
    //     }
    // });

    FOR_MAP(it, mInstancedMeshesMap)
    {
        it->second->enable();
        it->second->update();
        it->second->disable();
    }

    PROFILER_END_BLOCK()

    PROFILER_BLOCK_CPU(updateModelMatricesBuffer);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).setDataArray(mMatrices);
    PROFILER_END_BLOCK()

    GET_SYSTEM(MaterialManager).update();
	GET_SYSTEM(GPUSkeletalAnimationManager).update();
}

void RenderPipeline::processRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
	PROFILER_CPU()
    if(!renderer->isStatic())
    {
        renderer->update();
        setRendererMatrix(renderer);
    }
}

void RenderPipeline::terminate()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}

    mRenderInstancesSlotsManager.reset();
}

void RenderPipeline::addRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(mRenderInstancesSlotsManager.isEmpty())
    {
        mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        mMatrices.resize(mRenderInstancesSlotsManager.getSize());
        GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
    }

    renderer->setRenderSlot(mRenderInstancesSlotsManager.requestSlot());
    if(renderer->isStatic())
    {
        setRendererMatrix(renderer);
        mRenderersStatic.at(renderer->getRenderSlot().getSlot()) = renderer;
    }
    else
    {
        mUsedSlots.insert(renderer->getRenderSlot().getSlot());
        mRenderers.at(renderer->getRenderSlot().getSlot()) = renderer;
    }
    

    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            mRenderPassMap.at(*it)->addRenderer(renderer);
        }
    }

    InstancedMeshData instancedMeshData;
    instancedMeshData.init(renderer);
    if(!mInstancedMeshesMap.contains(instancedMeshData))
    {
        LOG_TRACE_MSG("New Batch")
        mInstancedMeshesMap.insert_or_assign(instancedMeshData, OwnerPtr<InstancedMeshRenderer>::newObject());
        mInstancedMeshesMap.at(instancedMeshData)->init(instancedMeshData);
    }

    mInstancedMeshesMap.at(instancedMeshData)->addRenderer(renderer);
}

void RenderPipeline::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(renderer->isStatic())
    {
        mRenderersStatic.at(renderer->getRenderSlot().getSlot()).reset();
    }
    else
    {
        mUsedSlots.erase(renderer->getRenderSlot().getSlot());
        mRenderers.at(renderer->getRenderSlot().getSlot()).reset();
    }

    mRenderInstancesSlotsManager.freeSlot(renderer->getRenderSlot());

    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            mRenderPassMap.at(*it)->removeRenderer(renderer);
        }
    }

    InstancedMeshData instancedMeshData;
    instancedMeshData.init(renderer);
    mInstancedMeshesMap.at(instancedMeshData)->removeRenderer(renderer);
}

void RenderPipeline::render(RenderPipelineData& renderData)
{
}

void RenderPipeline::frameAcquisition()
{
    /*
    * Frame acquisition
    */

    // Wait until the previous frame has finished
    constexpr uint32_t fenceCount = 1;
    constexpr VkBool32 waitForAllFences = VK_TRUE;
    constexpr uint64_t waitForFenceTimeout = UINT64_MAX;
    VkFence inFlightFence = GET_SYSTEM(GPUInstance).mGPUContext->inFlightFences[GET_SYSTEM(GPUInstance).mGPUContext->currentFrame];
    vkWaitForFences(GET_SYSTEM(GPUInstance).mGPUContext->vulkanDevice->getDevice(), fenceCount, &inFlightFence, waitForAllFences, waitForFenceTimeout);

    // Acquire an image from the swap chain
    uint32_t swapChainImageIndex;
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

void RenderPipeline::updateLights(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    LightBuiltIn::LightsData lightsData;
    FOR_ARRAY(i, renderData.mPointLights)
    {
        lightsData.mPointLights[i] = renderData.mPointLights[i]->calculateLightData();
    }

    if(renderData.mDirectionalLight)
    {
        lightsData.mDirectionalLight = renderData.mDirectionalLight->calculateLightData();
    }

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}

void RenderPipeline::initBuffers()
{
    // CPU BUFFERS

    mRenderInstancesSlotsManager.init(mInitialInstances);
    mRenderers.resize(mRenderInstancesSlotsManager.getSize());
    mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
    mMatrices.resize(mRenderInstancesSlotsManager.getSize());

    // GPU BUFFERS

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData, false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(LightBuiltIn::mLightsBufferData, false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(LightBuiltIn::mShadowMappingBufferData, false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices, false);

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().create();

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData).resize<GPUBuiltIn::UniformBuffers::GPUGlobalData>(1);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(LightBuiltIn::mLightsBufferData).resize<LightBuiltIn::LightsData>(1);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(LightBuiltIn::mShadowMappingBufferData).resize<LightBuiltIn::ShadowMappingData>(1);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}

void RenderPipeline::setRendererMatrix(TypedComponentHandler<MeshRenderer> renderer)
{
    PROFILER_CPU()
    if(renderer->getUpdateMatrix())
    {
        const Matrix4& rendererModelMatrix = renderer->getRendererModelMatrix();
        CHECK_MSG(mRenderInstancesSlotsManager.checkSlot(renderer->getRenderSlot()), "Invalid slot!");
        mMatrices.at(renderer->getRenderSlot().getSlot()) = rendererModelMatrix;
        renderer->setUpdateMatrix(false);
    }
}