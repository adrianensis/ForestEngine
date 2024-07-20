#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassGeometry: public RenderPass
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
protected:
    virtual void updateGlobalData() override;
    virtual void bindShader(const InstancedMeshData& instancedMeshData) override;
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
public:
    TypedComponentHandler<DirectionalLight> mDirectionalLight;
};
REGISTER_CLASS(RenderPassGeometry);
