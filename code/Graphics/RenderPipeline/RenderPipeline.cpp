#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void RenderPipeline::init()
{
    PROFILER_CPU()

    initBuffers();
}

void RenderPipeline::update()
{
	PROFILER_CPU()
    PROFILER_CPU_NAMED(updateRenderers);
    if(!mUsedSlots.empty())
    {
        FOR_RANGE(i, *mUsedSlots.begin(), (*mUsedSlots.rbegin())+1)
        {
            TComponentPtr<MeshRenderer> renderer = mRenderers[i];
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
    // [this](TComponentPtr<MeshRenderer> renderer)
    // {
    //     if(renderer.isValid())
    //     {
    //         processRenderer(renderer);
    //     }
    // });

    VkCommandBuffer vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->beginSingleTimeCommands();
    FOR_MAP(it, mGPUInstanceRendereresMap)
    {
        // it->second->enable();
        it->second->update(vulkanCommandBuffer);
        // it->second->disable();
    }
    GET_SYSTEM(GPUInstance).mGPUContext->endSingleTimeCommands(vulkanCommandBuffer, VK_NULL_HANDLE);

    PROFILER_CPU_NAMED(updateModelMatricesBuffer);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).setDataArray(mMatrices);

    GET_SYSTEM(GPUShaderManager).update();
	GET_SYSTEM(GPUSkeletalAnimationManager).update();
}

void RenderPipeline::processRenderer(TComponentPtr<MeshRenderer> renderer)
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
    FOR_MAP(it, mGPUShaderPipelines)
    {
        it->second->terminate();
    }

    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}
    FOR_MAP(it, mGPUInstanceRendereresMap)
	{
        it->second->terminate();
	}

    mRenderInstancesSlotsManager.reset();
}

void RenderPipeline::onResize()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->onResize();
	}
}

void RenderPipeline::addRenderer(TComponentPtr<MeshRenderer> renderer)
{
    PROFILER_CPU()
    bool compileShader = false;
    if(mRenderInstancesSlotsManager.isEmpty())
    {
        mRenderInstancesSlotsManager.increaseSize(mInitialInstances);
        mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
        mRenderers.resize(mRenderInstancesSlotsManager.getSize());
        mMatrices.resize(mRenderInstancesSlotsManager.getSize());
        // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).resize(sizeof(Matrix4) * mRenderInstancesSlotsManager.getSize());
        // compileShader = true;
    }

    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());
    if(!mGPUInstanceRendereresMap.contains(gpuInstanceRendererData))
    {
        PROFILER_CPU_NAMED(init_instanced_mesh)

        mGPUInstanceRendereresMap.insert_or_assign(gpuInstanceRendererData, OwnerPtr<GPUInstanceRenderer>::newObject());
        mGPUInstanceRendereresMap.at(gpuInstanceRendererData)->init(gpuInstanceRendererData);

        compileShader = true;
    }

    mGPUInstanceRendereresMap.at(gpuInstanceRendererData)->addRenderer(renderer->getGPURenderItem());

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
            Ptr<RenderPass> renderPass = mRenderPassMap.at(*it);
            renderPass->addRenderer(renderer);

            if(compileShader)
            {
                if(mGPUShaderPipelines.contains(gpuInstanceRendererData))
                {
                    mGPUShaderPipelines.at(gpuInstanceRendererData)->terminate();
                    mGPUShaderPipelines.at(gpuInstanceRendererData).invalidate();
                }

                mGPUShaderPipelines.emplace(gpuInstanceRendererData, renderPass->compileShader(renderer));
            }
        }
    }
}

void RenderPipeline::removeRenderer(TComponentPtr<MeshRenderer> renderer)
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

    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());
    mGPUInstanceRendereresMap.at(gpuInstanceRendererData)->removeRenderer(renderer->getGPURenderItem());
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
    // CPU BUFFERS

    mRenderInstancesSlotsManager.init(mInitialInstances * 100);
    mRenderers.resize(mRenderInstancesSlotsManager.getSize());
    mRenderersStatic.resize(mRenderInstancesSlotsManager.getSize());
    mMatrices.resize(mRenderInstancesSlotsManager.getSize());

    // GPU BUFFERS

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mLightsBufferData, sizeof(GPULightBuiltIn::LightsData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData, sizeof(GPULightBuiltIn::ShadowMappingData), false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices, sizeof(Matrix4) * mRenderInstancesSlotsManager.getSize(), false);

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).resize<GPUShaderDefinitions::UniformBuffers::GPUGlobalData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).resize<GPULightBuiltIn::LightsData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData).resize<GPULightBuiltIn::ShadowMappingData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}

void RenderPipeline::setRendererMatrix(TComponentPtr<MeshRenderer> renderer)
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