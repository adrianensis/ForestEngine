#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/Material/MaterialRuntime/MaterialRuntimeShadowMap.hpp"

class RenderPassShadowMap: public RenderPass
{
public:
    virtual void init(const RenderPassData& renderPassData);
protected:
    virtual void postFramebufferEnabled() override;
    virtual void render() override;
    virtual void updateGlobalData() override;
    virtual void initShader(Ptr<BatchRenderer> batch);
public:
    Ptr<PointLight> mPointLight;
private:
    OwnerPtr<MaterialRuntimeShadowMap> mMaterialRuntime;
};
REGISTER_CLASS(RenderPassShadowMap);
