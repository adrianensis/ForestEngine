#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/RenderGraph/GPURenderGraph.hpp"
#include "Graphics/MeshRenderer/MeshRenderer.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "GPU/RenderItem/GPURenderItemManager.hpp"
#include "GPU/GPUInstance.hpp"

class RenderPipelineData
{
public:
    std::vector<EC::TComponentPtr<PointLight>> mPointLights;
    EC::TComponentPtr<DirectionalLight> mDirectionalLight;
};

class RenderPipeline
{
public:
    virtual void init();
    virtual ~RenderPipeline() = default;
    void update();
    virtual void terminate();
    void addRenderer(EC::TComponentPtr<MeshRenderer> renderer);
    void removeRenderer(EC::TComponentPtr<MeshRenderer> renderer);
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
    std::vector<EC::TComponentPtr<MeshRenderer>> mMeshRenderers;
};
REGISTER_CLASS(RenderPipeline);
