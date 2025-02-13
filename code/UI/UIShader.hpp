#pragma once

#include "GPU/Shader/BuiltIn/GPUShaderDefault.hpp"

class GPUShaderPropertiesBlockUI
{
public:
    Vector4 mColor = Vector4(0,0,0,1);
    alignas(16) Vector2 mTextureRegionLeftTop = Vector2(0.0, 0.0);
    Vector2 mTextureRegionSize = Vector2(1.0, 1.0);
    alignas(16) i32 mDepth = 0;
};
REGISTER_CLASS(GPUShaderPropertiesBlockUI)

class GPUShaderUI : public GPUShaderDefault
{
protected:
    virtual void setSharedGPUShaderPropertiesBlock() override;
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateGPUShaderPropertiesBlock() override;

    virtual void fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const override;
    virtual void vertexGPUShaderCalculateTextureCoordinateOutput(GPUShaderBuilder& GPUShaderBuilder) const override;
    virtual void vertexGPUShaderCalculatePositionOutputCustom(GPUShaderBuilder& GPUShaderBuilder) const override;
};
REGISTER_CLASS(GPUShaderUI)


class GPUShaderUIFont : public GPUShaderUI
{
protected:
    virtual void fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const override;
};
REGISTER_CLASS(GPUShaderUIFont)