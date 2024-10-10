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
    if(!mUsedSlots.empty())
    {
        FOR_RANGE(i, *mUsedSlots.begin(), (*mUsedSlots.rbegin())+1)
        {
            TypedComponentHandler<MeshRenderer> renderer = mRenderers[i];
            if(renderer.isValid())
            {
                processRenderer(renderer);
            }
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
        // it->second->enable();
        it->second->update();
        // it->second->disable();
    }

    PROFILER_END_BLOCK()

    PROFILER_BLOCK_CPU(updateModelMatricesBuffer);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).setDataArray(mMatrices);
    PROFILER_END_BLOCK()

    // GET_SYSTEM(MaterialManager).update();
	// GET_SYSTEM(GPUSkeletalAnimationManager).update();
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
        // mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        // mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        // mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        // mMatrices.resize(mRenderInstancesSlotsManager.getSize());
        // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
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

void RenderPipeline::compile()
{
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

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData, sizeof(GPUBuiltIn::UniformBuffers::GPUGlobalData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(LightBuiltIn::mLightsBufferData, sizeof(LightBuiltIn::LightsData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(LightBuiltIn::mShadowMappingBufferData, sizeof(LightBuiltIn::ShadowMappingData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices, sizeof(Matrix4) * mRenderInstancesSlotsManager.getSize(), false);

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().create();

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mGlobalData).resize<GPUBuiltIn::UniformBuffers::GPUGlobalData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(LightBuiltIn::mLightsBufferData).resize<LightBuiltIn::LightsData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(LightBuiltIn::mShadowMappingBufferData).resize<LightBuiltIn::ShadowMappingData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUBuiltIn::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
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