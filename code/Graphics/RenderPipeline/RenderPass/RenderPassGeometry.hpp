#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"
// TODO: Fix ComponentPtr includes everywhere
#include "Core/EntityComponent/ComponentPtr.hpp"
class RenderPassGeometry: public RenderPass
{
protected:
    virtual void updateGlobalData() override;
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
public:
    TComponentPtr<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassGeometry);
