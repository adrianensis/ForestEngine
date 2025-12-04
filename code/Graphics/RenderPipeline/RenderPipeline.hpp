#pragma once

#include "Engine/Core.hpp"
#include "Graphics/Light/Light.hpp"
#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "GPU/RenderItem/GPURenderItemManager.hpp"
#include "GPU/GPUInstance.hpp"

class RenderPipelineData
{
public:
    std::vector<PointLight*> mPointLights;
    DirectionalLight* mDirectionalLight = nullptr;
};

class RenderPipeline
{
public:
    virtual void init();
    virtual ~RenderPipeline() = default;
    void update();
    virtual void terminate();
    void addRenderer(MeshRenderer* renderer);
    void removeRenderer(MeshRenderer* renderer);
    void render(RenderPipelineData& renderData);
    virtual void compile();
    void onResize();

protected:
    void updateLights(RenderPipelineData& renderData);
    void initBuffers();

protected:
    GPURenderItemManager mGPURenderItemManager;
    Core::OwnerPtr<GPUInstanceRendererManager> mGPUInstanceRendererManager;
    GPURenderGraph mGPURenderGraph;
    std::vector<MeshRenderer*> mMeshRenderers;
    Core::OwnerPtr<GPUUniformBuffersContainer> mGlobalGPUUniformBuffersContainer;
};
REGISTER_CLASS(RenderPipeline);
