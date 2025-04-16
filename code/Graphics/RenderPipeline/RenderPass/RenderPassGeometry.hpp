#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassGeometry: public RenderPass
{
protected:
    virtual void updateGlobalData() override;
    virtual void preRender() override;
    virtual void render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass) override;
    virtual void postRender() override;
public:
    TComponentPtr<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassGeometry);
