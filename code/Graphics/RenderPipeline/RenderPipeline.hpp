#pragma once

#include "Engine/Core.hpp"
#include "GPU/Light/GPULight.hpp"
#include "Graphics/Camera/CameraManager.hpp"
#include "Graphics/Light/Light.hpp"
#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "GPU/RenderItem/GPURenderItemManager.hpp"
#include "GPU/GPUInstance.hpp"

class RenderPipelineUpdateData
{
public:
    std::vector<PointLightData> mPointLightsData;
    DirectionalLightData mDirectionalLightData;
    AmbientLightData mAmbientLightData;
};

class RenderPipeline
{
public:
    virtual void init(CameraManager* cameraManager);
    virtual ~RenderPipeline() = default;
    void update(Core::f32 dt, RenderPipelineUpdateData& renderPipelineUpdateData);
    virtual void terminate();
    void addRenderer(MeshRenderer* renderer);
    void removeRenderer(MeshRenderer* renderer);
    void render();
    virtual void compile();
    void onResize();

protected:
    void updateLights(RenderPipelineUpdateData& renderData);
    void initBuffers();

protected:
    GPURenderItemManager mGPURenderItemManager;
    Core::OwnerPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    GPURenderGraph mGPURenderGraph;
    std::vector<MeshRenderer*> mMeshRenderers;
    Core::OwnerPtr<GPUUniformBuffersContainer> mGlobalGPUUniformBuffersContainer;
    CameraManager* mCameraManager = nullptr;
};
REGISTER_CLASS(RenderPipeline);
