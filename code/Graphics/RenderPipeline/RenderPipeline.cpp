#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/GPU/GPUGlobalState.hpp"
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
    PROFILER_BLOCK_CPU("update renderers");
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

    PROFILER_END_BLOCK()

    PROFILER_BLOCK_CPU("update mModelMatrices buffer");
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).setDataArray(mMatrices);
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
    if(mRenderInstancesSlotsManager.isEmpty())
    {
        mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        mMatrices.resize(mRenderInstancesSlotsManager.getSize());
        GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
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
    if(!mBatchMap.contains(instancedMeshData))
    {
        LOG_TRACE_MSG("New Batch")
        mBatchMap.insert_or_assign(instancedMeshData, OwnerPtr<InstancedMeshRenderer>::newObject());
        mBatchMap.at(instancedMeshData)->init(instancedMeshData);
    }

    mBatchMap.at(instancedMeshData)->addRenderer(renderer);
}

void RenderPipeline::removeRenderer(TypedComponentHandler<MeshRenderer> renderer)
{
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
    mBatchMap.at(instancedMeshData)->removeRenderer(renderer);
}

void RenderPipeline::render(RenderPipelineData& renderData)
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

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}

void RenderPipeline::initBuffers()
{
    // CPU BUFFERS

    mRenderInstancesSlotsManager.init(mInitialInstances);
    mRenderers.resize(mRenderInstancesSlotsManager.getSize());
    mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
    mMatrices.resize(mRenderInstancesSlotsManager.getSize());

    // GPU BUFFERS

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().addSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData, false);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().addSharedBuffer(LightBuiltIn::mLightsBufferData, false);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().addSharedBuffer(LightBuiltIn::mShadowMappingBufferData, false);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().addSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices, false);

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().create();

    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mGlobalData).resize<GPUBuiltIn::SharedBuffers::GPUGlobalData>(1);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).resize<LightBuiltIn::LightsData>(1);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mShadowMappingBufferData).resize<LightBuiltIn::ShadowMappingData>(1);
    GET_SYSTEM(GPUGlobalState).getGPUSharedBuffersContainer().getSharedBuffer(GPUBuiltIn::SharedBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
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
