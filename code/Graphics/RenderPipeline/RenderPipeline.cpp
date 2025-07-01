#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void RenderPipeline::init()
{
    PROFILER_CPU()

    mGPURenderItemManager.init();
    initBuffers();

    mGPUInstanceRendererManager = Core::OwnerPtr<GPUInstanceRendererManager>::newObject();
    mGPURenderGraph.init(GET_SYSTEM(GPUInstance).mGPUContext, mGPUInstanceRendererManager);

    mMeshRenderers.resize(mGPURenderItemManager.getSize());
}

void RenderPipeline::update()
{
	PROFILER_CPU()

    if(mGPURenderItemManager.getSize() > mMeshRenderers.size())
    {
        mMeshRenderers.resize(mGPURenderItemManager.getSize());
    }

    FOR_RANGE(i, *mGPURenderItemManager.getUsedSlots().begin(), (*mGPURenderItemManager.getUsedSlots().rbegin())+1)
    {
        EC::TComponentPtr<MeshRenderer> renderItem = mMeshRenderers[i];
        if(mMeshRenderers[i].isValid())
        {
            mMeshRenderers[i]->update();
        }
    }

    mGPURenderItemManager.update();
    mGPUInstanceRendererManager->update(GET_SYSTEM(GPUInstance).mGPUContext);
    mGPURenderGraph.update();

    PROFILER_CPU_NAMED(updateModelMatricesBuffer);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).setDataArray(mGPURenderItemManager.getMatrices());

    GET_SYSTEM(GPUShaderManager).update();
	GET_SYSTEM(GPUSkeletalAnimationManager).update();
}

void RenderPipeline::terminate()
{
    mGPURenderGraph.terminate();
    mGPUInstanceRendererManager->terminate();
    mGPUInstanceRendererManager.invalidate();
    mGPURenderItemManager.terminate();
}

void RenderPipeline::onResize()
{
    mGPURenderGraph.onResize();
}

void RenderPipeline::addRenderer(EC::TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    mGPURenderItemManager.addRenderer(renderer->getGPURenderItem());

    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());
    mGPUInstanceRendererManager->addInstanceRenderer(gpuInstanceRendererData);
    mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData)->addRenderer(renderer->getGPURenderItem());
    
    mGPURenderGraph.addRenderer(renderer->getGPURenderItem());

    mMeshRenderers[renderer->getGPURenderItem()->getRenderSlot().getSlot()] = renderer;
}

void RenderPipeline::removeRenderer(EC::TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()

    Core::u32 slot = renderer->getGPURenderItem()->getRenderSlot().getSlot();
    
    mGPURenderItemManager.removeRenderer(renderer->getGPURenderItem());
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());
    mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData)->removeRenderer(renderer->getGPURenderItem());
    mGPURenderGraph.removeRenderer(renderer->getGPURenderItem());
    
    mMeshRenderers[slot].reset();
}

void RenderPipeline::render(RenderPipelineData& renderData)
{
    GPURenderGraphData data;
    mGPURenderGraph.render(data);
}

void RenderPipeline::compile()
{
}

void RenderPipeline::updateLights(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    GPULightBuiltIn::LightsData lightsData;
    FOR_ARRAY(i, renderData.mPointLights)
    {
        lightsData.mPointLights[i] = renderData.mPointLights[i]->calculateLightData();
    }

    if(renderData.mDirectionalLight)
    {
        lightsData.mDirectionalLight = renderData.mDirectionalLight->calculateLightData();
    }

    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).setData(lightsData);
}

void RenderPipeline::initBuffers()
{
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mLightsBufferData, sizeof(GPULightBuiltIn::LightsData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData, sizeof(GPULightBuiltIn::ShadowMappingData), false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices, sizeof(Matrix4) * mGPURenderItemManager.getSize(), false);

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).resize<GPUShaderDefinitions::UniformBuffers::GPUGlobalData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).resize<GPULightBuiltIn::LightsData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData).resize<GPULightBuiltIn::ShadowMappingData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}