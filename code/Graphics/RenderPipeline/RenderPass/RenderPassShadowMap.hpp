#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPassBase.hpp"
#include "Graphics/Light/Light.hpp"
#include "Core/EntityComponent/Component.hpp"

class RenderPassShadowMap: public RenderPassBase
{
protected:
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
    virtual void updateGlobalData() override;
public:
    DirectionalLight* mDirectionalLight = nullptr;
};
REGISTER_CLASS(RenderPassShadowMap);
