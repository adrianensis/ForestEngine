#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"

class RenderPassShadowMap: public RenderPass
{
public:
    virtual void init(Ptr<RenderPipeline> renderPipeline, const RenderPassData& renderPassData);
protected:
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
    virtual void updateGlobalData() override;
    virtual Ptr<Shader> getShader(const BatchData& batchData) const;
public:
    Ptr<DirectionalLight> mDirectionalLight;
private:
    Ptr<Shader> mShaderShadowMap;
};
REGISTER_CLASS(RenderPassShadowMap);
