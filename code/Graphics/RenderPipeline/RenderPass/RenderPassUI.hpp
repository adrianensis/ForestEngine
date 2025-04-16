#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPassUI: public RenderPass
{
protected:
    virtual void preRender() override;
    virtual void render(const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass) override;
    virtual void postRender() override;
private:
    void renderStencilCascade(u64 id, const std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>& gpuInstanceRendererDataByRenderPass);
private:
    std::unordered_set<u64> mStencilsRendered;
};
REGISTER_CLASS(RenderPassUI);
