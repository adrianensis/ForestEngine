#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Graphics/Renderer/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Core/Window/WindowManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/Shader/ShaderPBR.hpp"

void RenderPipelinePBR::compile()
{
    LOG_TRACE()
    PROFILER_CPU()

    // RenderPassData renderPassShadowMapData;
    // renderPassShadowMapData.mOutputFramebufferData.set(
    //     {
    //         GPUFramebufferAttachmentType::DEPTH
    //     },
    //     2048, 2048
    // );
    // initRenderPass<RenderPassShadowMap>(renderPassShadowMapData);

    // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
    RenderPassData renderPassGeometryData;
    // renderPassGeometryData.mShader = GET_SYSTEM(ShaderManager).createShader<ShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(ShaderManager).createShader<ShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    initRenderPass<RenderPassGeometry>(renderPassGeometryData);
    RenderPassData renderPassUIData;
    renderPassUIData.mGeometricSpace = GeometricSpace::SCREEN;
    initRenderPass<RenderPassUI>(renderPassUIData);
}


void RenderPipelinePBR::render(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    vulkanRenderPass->begin();
    {
        PROFILER_GPU_NAMED(renderPass, vulkanRenderPass->mGPUContext->mTracyContext, vulkanRenderPass->mGPUContext->vulkanCommandBuffers[vulkanRenderPass->mGPUContext->currentFrame]->getVkCommandBuffer())
        // updateLights(renderData);

    //	GET_SYSTEM(GPUInterface).clear();

        // FOR_ARRAY(i, renderData.mPointLights)
        {
            // WeakPtr<PointLight> pointLight = renderData.mPointLights.at(i);
            // WeakPtr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
            // renderPassShadowMap->mDirectionalLight = renderData.mDirectionalLight;
            // renderPassShadowMap->renderPass();
            WeakPtr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
            renderPassGeometry->mDirectionalLight = renderData.mDirectionalLight;
            renderPassGeometry->renderPass();
        }
        // WeakPtr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
        // renderPassGeometry->mPointLight = renderData.mPointLights[0];
        // renderPassGeometry->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();

        // vulkanRenderPass->clearColor();
        // vulkanRenderPass->clearDepthStencil();

        WeakPtr<RenderPassUI> renderPassUI = getRenderPass<RenderPassUI>();
        renderPassUI->renderPass();

        // GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();
    }
    vulkanRenderPass->end();
}