#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPassShadowMap: public RenderPass
{
protected:
    virtual void postFramebufferEnabled() override;
    virtual void render() override;
};
REGISTER_CLASS(RenderPassShadowMap);
