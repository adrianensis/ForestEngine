#pragma once

#include "GPU/Shader/BuiltIn/GPUShaderDefault.hpp"

class GPUShaderPropertiesBlockUI
{
public:
    Maths::Vector4 mColor = Maths::Vector4(0,0,0,1);
    alignas(16) Maths::Vector2 mTextureRegionLeftTop = Maths::Vector2(0.0, 0.0);
    Maths::Vector2 mTextureRegionSize = Maths::Vector2(1.0, 1.0);
    alignas(16) Core::i32 mDepth = 0;
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