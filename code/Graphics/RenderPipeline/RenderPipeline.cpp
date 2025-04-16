#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "GPU/GPUInstance.hpp"
#include "Scene/Module.hpp"
#include "GPU/Shader/GPUShaderManager.hpp"
#include "GPU/SkeletalAnimation/GPUSkeletalAnimationManager.hpp"

void RenderPipeline::init()
{
    PROFILER_CPU()

    mMeshRendererManager.init();
    initBuffers();
}

void RenderPipeline::update()
{
	PROFILER_CPU()

    mMeshRendererManager.update();

    VkCommandBuffer vulkanCommandBuffer = GET_SYSTEM(GPUInstance).mGPUContext->beginSingleTimeCommands();
    FOR_MAP(it, mGPUInstanceRendereresMap)
    {
        // it->second->enable();
        it->second->update(vulkanCommandBuffer);
        // it->second->disable();
    }
    GET_SYSTEM(GPUInstance).mGPUContext->endSingleTimeCommands(vulkanCommandBuffer, VK_NULL_HANDLE);

    PROFILER_CPU_NAMED(updateModelMatricesBuffer);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).setDataArray(mMeshRendererManager.getMatrices());

    GET_SYSTEM(GPUShaderManager).update();
	GET_SYSTEM(GPUSkeletalAnimationManager).update();
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

    mMeshRendererManager.terminate();
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

    mMeshRendererManager.addRenderer(renderer);

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
    
    FOR_LIST(it, renderer->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            mGPUInstanceRendererDataByRenderPass.at(*it).insert(gpuInstanceRendererData);
            
            Ptr<RenderPass> renderPass = mRenderPassMap.at(*it);
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
    GPUInstanceRendererData gpuInstanceRendererData;
    gpuInstanceRendererData.init(renderer->getGPURenderItem());

    mMeshRendererManager.removeRenderer(renderer);

    FOR_LIST(it, renderer->getGPURenderItemData().mRenderPassIDs)
    {
        if(mRenderPassMap.contains(*it))
        {
            // TODO: Only remove if renderers count == 0
            // mGPUInstanceRendererDataByRenderPass.at(*it).erase(gpuInstanceRendererData);
        }
    }

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
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData, sizeof(GPUShaderDefinitions::UniformBuffers::GPUGlobalData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mLightsBufferData, sizeof(GPULightBuiltIn::LightsData), false);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData, sizeof(GPULightBuiltIn::ShadowMappingData), false);
    GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().addUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices, sizeof(Matrix4) * mMeshRendererManager.getSize(), false);

    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mGlobalData).resize<GPUShaderDefinitions::UniformBuffers::GPUGlobalData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mLightsBufferData).resize<GPULightBuiltIn::LightsData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPULightBuiltIn::mShadowMappingBufferData).resize<GPULightBuiltIn::ShadowMappingData>(1);
    // GET_SYSTEM(GPUInstance).getGPUUniformBuffersContainer().getUniformBuffer(GPUShaderDefinitions::UniformBuffers::mModelMatrices).resize<Matrix4>(mRenderInstancesSlotsManager.getSize());
}