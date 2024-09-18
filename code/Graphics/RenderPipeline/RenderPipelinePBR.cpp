#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"
#include "Graphics/Renderer/ShapeBatchRenderer.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Scene/Module.hpp"
#include "Graphics/Window/WindowManager.hpp"
#include "Graphics/Debug/DebugRenderer.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassGeometry.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassShadowMap.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPassUI.hpp"
#include "Graphics/Material/Shader/ShaderPBR.hpp"
#include "Graphics/Material/Shader/ShaderManager.hpp"

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

    // Ptr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
    RenderPassData renderPassGeometryData;
    renderPassGeometryData.mShader = GET_SYSTEM(ShaderManager).createShader<ShaderDefault>();
    // renderPassGeometryData.mShader = GET_SYSTEM(ShaderManager).createShader<ShaderPBR>();
    // renderPassGeometryData.mDependencies.push_back(RenderPassDependency{TextureBindingNamesPBR::smShadowMap,
    // GPUFramebufferAttachmentType::DEPTH, renderPassShadowMap, GPUPipelineStage::FRAGMENT});
    initRenderPass<RenderPassGeometry>(renderPassGeometryData);
    // RenderPassData renderPassUIData;
    // renderPassUIData.mGeometricSpace = GeometricSpace::SCREEN;
    // initRenderPass<RenderPassUI>(renderPassUIData);
}


void RenderPipelinePBR::render(RenderPipelineData& renderData)
{
	PROFILER_CPU()

    // updateLights(renderData);

//	GET_SYSTEM(GPUInterface).clear();

    // FOR_ARRAY(i, renderData.mPointLights)
    {
        // Ptr<PointLight> pointLight = renderData.mPointLights.at(i);
        // Ptr<RenderPassShadowMap> renderPassShadowMap = getRenderPass<RenderPassShadowMap>();
        // renderPassShadowMap->mDirectionalLight = renderData.mDirectionalLight;
        // renderPassShadowMap->renderPass();
        Ptr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
        renderPassGeometry->mDirectionalLight = renderData.mDirectionalLight;
        renderPassGeometry->renderPass();
    }
    // Ptr<RenderPassGeometry> renderPassGeometry = getRenderPass<RenderPassGeometry>();
    // renderPassGeometry->mPointLight = renderData.mPointLights[0];
    // renderPassGeometry->renderPass();

	// GET_SYSTEM(DebugRenderer).mShapeBatchRenderer.render();

//    GET_SYSTEM(GPUInterface).clearDepth();
//    GET_SYSTEM(GPUInterface).clearStencil();

    // Ptr<RenderPassUI> renderPassUI = getRenderPass<RenderPassUI>();
    // renderPassUI->renderPass();

	// GET_SYSTEM(DebugRenderer).mShapeBatchRendererScreenSpace.render();
}