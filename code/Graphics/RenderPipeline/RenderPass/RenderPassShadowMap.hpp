#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassShadowMap: public RenderPass
{
protected:
    virtual void preRender() override;
    virtual void render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass) override;
    virtual void postRender() override;
    virtual void updateGlobalData() override;
public:
    TComponentPtr<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassShadowMap);
