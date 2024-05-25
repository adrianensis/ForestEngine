#include "Graphics/RenderPipeline/RenderPipeline.hpp"
#include "Graphics/Renderer/BatchRenderer/ShapeBatchRenderer.hpp"
#include "Graphics/GPU/GPUInterface.hpp"
#include "Graphics/RenderSharedContext.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Window/Window.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"

void RenderPipeline::init()
{
    RenderPassData renderPassShadowMapData;
    renderPassShadowMapData.mOutputFramebufferData.set(
        {
            GPUFramebufferAttachmentType::DEPTH
        },
        GET_SYSTEM(Window).getWindowSize().x, GET_SYSTEM(Window).getWindowSize().y
    );
    initRenderPass<RenderPassShadowMap>(renderPassShadowMapData);

    Ptr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
    RenderPassData renderPassGeometryData;
    renderPassGeometryData.mDependencies.push_back(FramebufferBinding{TextureBindingNamesPBR::smShadowMap,
    renderPassShadowMap->getOutputGPUFramebuffer().getAttachments().at(GPUFramebufferAttachmentType::DEPTH).mAttachmentID, GPUPipelineStage::FRAGMENT});
    initRenderPass<RenderPassGeometry>(renderPassGeometryData);
    RenderPassData renderPassUIData;
    renderPassUIData.mGeometricSpace = GeometricSpace::SCREEN;
    initRenderPass<RenderPassUI>(renderPassUIData);
}

void RenderPipeline::terminate()
{
    FOR_MAP(it, mRenderPassMap)
	{
        it->second->terminate();
	}
}

void RenderPipeline::addRenderer(Ptr<MeshRenderer> renderer)
{
    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        CHECK_MSG(mRenderPassMap.contains(*it), "RenderPass not found!");
        mRenderPassMap.at(*it)->addRenderer(renderer);
    }
}

void RenderPipeline::removeRenderer(Ptr<MeshRenderer> renderer)
{
    FOR_LIST(it, renderer->getRendererData().mRenderPassIDs)
    {
        CHECK_MSG(mRenderPassMap.contains(*it), "RenderPass not found!");
        mRenderPassMap.at(*it)->removeRenderer(renderer);
    }
}

void RenderPipeline::render(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    updateLights(renderData);

	GET_SYSTEM(GPUInterface).clear();

    // FOR_ARRAY(i, renderData.mPointLights)
    {
        // Ptr<PointLight> pointLight = renderData.mPointLights.at(i);
        Ptr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
        renderPassShadowMap->mDirectionalLight = renderData.mDirectionalLight;
        renderPassShadowMap->renderPass();
        Ptr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
        renderPassGeometry->mDirectionalLight = renderData.mDirectionalLight;
        renderPassGeometry->renderPass();
    }
    // Ptr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
    // renderPassGeometry->mPointLight = renderData.mPointLights[0];
    // renderPassGeometry->renderPass();

	GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();
    
    GET_SYSTEM(GPUInterface).clearDepth();
    GET_SYSTEM(GPUInterface).clearStencil();

    Ptr<RenderPassUI> renderPassUI = getRenderPass<RenderPassUI>();
    renderPassUI->renderPass();

	GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();
}

void RenderPipeline::updateLights(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    LightBuiltIn::LightsData lightsData;
    FOR_ARRAY(i, renderData.mPointLights)
    {
        lightsData.mPointLights[i] = renderData.mPointLights[i]->getLightData();
    }

    if(renderData.mDirectionalLight)
    {
        lightsData.mDirectionalLight = renderData.mDirectionalLight->getLightData();
    }

    GET_SYSTEM(RenderSharedContext).getGPUSharedBuffersContainer().getSharedBuffer(LightBuiltIn::mLightsBufferData).setData(lightsData);
}