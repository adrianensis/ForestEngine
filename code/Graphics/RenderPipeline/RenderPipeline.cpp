#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void RenderPipeline::init()
{
    PROFILER_CPU()

    mGPURenderItemManager.init();
    mGlobalGPUUniformBuffersContainer = Core::OwnerPtr<GPUUniformBuffersContainer>::newObject();
    initBuffers();

    mGPUInstanceRendererManager = Core::OwnerPtr<GPUInstanceRendererManager>::newObject();
    mGPURenderGraph.init(GPUInstance::getInstance().mGPUContext, mGPUInstanceRendererManager, mGlobalGPUUniformBuffersContainer,
        GPUInstance::getInstance().mGPUSkeletalAnimationManager, GPUInstance::getInstance().mGPUShaderManager);

    mMeshRenderers.resize(mGPURenderItemManager.getSize());
}

void RenderPipeline::update()
{
	PROFILER_CPU()

    if(mGPURenderItemManager.getSize() > mMeshRenderers.size())
    {
        mMeshRenderers.resize(mGPURenderItemManager.getSize());
    }

    if(!mGPURenderItemManager.getUsedSlots().empty())
    {
        FOR_RANGE(i, *mGPURenderItemManager.getUsedSlots().begin(), (*mGPURenderItemManager.getUsedSlots().rbegin())+1)
        {
            EC::ComponentPtr<MeshRenderer> renderItem = mMeshRenderers[i];
            if(mMeshRenderers[i].isValid())
            {
                mMeshRenderers[i]->update();
            }
        }
    }

    mGPURenderItemManager.update();
    mGPUInstanceRendererManager->update(GPUInstance::getInstance().mGPUContext);
    mGPURenderGraph.update();

    PROFILER_CPU_NAMED(updateModelMatricesBuffer);
    mGlobalGPUUniformBuffersContainer->getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).setDataArray(mGPURenderItemManager.getMatrices());

    GPUInstance::getInstance().mGPUShaderManager->update();
	GPUInstance::getInstance().mGPUSkeletalAnimationManager->update();
}

void RenderPipeline::terminate()
{
    mGPURenderGraph.terminate();
    mGPUInstanceRendererManager->terminate();
    mGPUInstanceRendererManager.invalidate();
    mGlobalGPUUniformBuffersContainer->terminate();
    mGlobalGPUUniformBuffersContainer.invalidate();
    mGPURenderItemManager.terminate();
}

void RenderPipeline::onResize()
{
    mGPURenderGraph.onResize();
}

void RenderPipeline::addRenderer(EC::ComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    mGPURenderItemManager.addRenderer(renderer->getGPURenderItem());

    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());
    mGPUInstanceRendererManager->addInstanceRenderer(GPUInstance::getInstance().mGPUContext, gpuInstanceRendererData);
    mGPUInstanceRendererManager->getInstanceRenderer(gpuInstanceRendererData)->addRenderer(renderer->getGPURenderItem());
    
    mGPURenderGraph.addRenderer(renderer->getGPURenderItem());

    mMeshRenderers[renderer->getGPURenderItem()->getRenderSlot().getSlot()] = renderer;
}

void RenderPipeline::removeRenderer(EC::ComponentPtr<MeshRenderer> renderer)
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

    mGlobalGPUUniformBuffersContainer->getUniformBuffer(GPULightBuiltIn::mLightsBufferData).setData(lightsData);
}

void RenderPipeline::initBuffers()
{
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mLightsBufferData, sizeof(GPULightBuiltIn::LightsData), false);
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData, sizeof(GPULightBuiltIn::ShadowMappingData), false);
    mGlobalGPUUniformBuffersContainer->addUniformBuffer(GPUInstance::getInstance().mGPUContext, GPUShaderDefinitions::UniformBuffers::mModelMatrices, sizeof(Maths::Matrix4) * mGPURenderItemManager.getSize(), false);

    // GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).resize<GPUShaderDefinitions::UniformBuffers::GPUGlobalData>(1);
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).resize<GPULightBuiltIn::LightsData>(1);
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData).resize<GPULightBuiltIn::ShadowMappingData>(1);
    // GPUInstance::getInstance().getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).resize<Maths::Matrix4>(mRenderInstancesSlotsManager.getSize());
}