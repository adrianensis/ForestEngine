#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Material/Shader/ShaderDepthBuffer.hpp"

class RenderPassShadowMap: public RenderPass
{
public:
    virtual void init(const RenderPassData& renderPassData);
protected:
    virtual void preFramebufferEnabled() override;
    virtual void postFramebufferEnabled() override;
    virtual void preRender() override;
    virtual void render() override;
    virtual void postRender() override;
    virtual void updateGlobalData() override;
    virtual Ptr<Shader> getShader(const BatchData& batchData) const override;
public:
    Ptr<DirectionalLight> mDirectionalLight;
private:
    OwnerPtr<ShaderDepthBuffer> mShaderDepthBuffer;
};
REGISTER_CLASS(RenderPassShadowMap);
