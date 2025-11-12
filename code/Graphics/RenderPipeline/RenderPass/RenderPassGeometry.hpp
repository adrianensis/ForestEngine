#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPassBase.hpp"
#include "Graphics/Light/Light.hpp"
#include "Core/EntityComponent/Component.hpp"
class RenderPassGeometry: public RenderPassBase
{
protected:
    virtual void updateGlobalData() override;
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
public:
    EC::ComponentPtr<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassGeometry);
