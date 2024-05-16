#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassShadowMap: public RenderPass
{
protected:
    virtual void postFramebufferEnabled() override;
    virtual void render() override;
    virtual void updateGlobalData() override;
public:
    Ptr<PointLight> mPointLight;
};
REGISTER_CLASS(RenderPassShadowMap);
