#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPassUI: public RenderPass
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
private:
    void renderStencilCascade(u64 id);
private:
    std::unordered_set<u64> mStencilsRendered;
};
REGISTER_CLASS(RenderPassUI);
