#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPassBase.hpp"

class RenderPassUI: public RenderPassBase
{
protected:
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
private:
    void renderStencilCascade(Core::u64 id);
private:
    std::unordered_set<Core::u64> mStencilsRendered;
};
REGISTER_CLASS(RenderPassUI);
