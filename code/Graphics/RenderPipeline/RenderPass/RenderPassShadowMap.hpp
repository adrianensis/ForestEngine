#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Material/Shader/ShaderShadowMap.hpp"

class RenderPassShadowMap: public RenderPass
{
public:
    virtual void init(const RenderPassData& renderPassData);
protected:
    virtual void postFramebufferEnabled() override;
    virtual void render() override;
    virtual void updateGlobalData() override;
    virtual Ptr<const Shader> getShader(const BatchData& batchData) const;
public:
    Ptr<PointLight> mPointLight;
private:
    OwnerPtr<ShaderShadowMap> mShader;
};
REGISTER_CLASS(RenderPassShadowMap);
