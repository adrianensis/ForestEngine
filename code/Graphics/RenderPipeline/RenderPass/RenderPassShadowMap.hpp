#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"
// TODO: Fix ComponentPtr includes everywhere
#include "Core/EntityComponent/ComponentPtr.hpp"

class RenderPassShadowMap: public RenderPass
{
protected:
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
    virtual void updateGlobalData() override;
public:
    TComponentPtr<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassShadowMap);
