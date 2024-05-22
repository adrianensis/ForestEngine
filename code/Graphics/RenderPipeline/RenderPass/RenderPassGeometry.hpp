#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassGeometry: public RenderPass
{
protected:
    virtual void updateGlobalData() override;
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
public:
    Ptr<PointLight> mPointLight;
};
REGISTER_CLASS(RenderPassGeometry);
