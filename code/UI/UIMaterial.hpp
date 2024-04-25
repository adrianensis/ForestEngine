#pragma once

#include "Graphics/Material/MaterialRuntime/MaterialRuntimeDefault.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialPropertiesBlockUI
{
public:
    Vector4 mColor = Vector4(0,0,0,1);
    alignas(16) Vector2 mTextureRegionLeftTop = Vector2(0.0, 0.0);
    Vector2 mTextureRegionSize = Vector2(1.0, 1.0);
    alignas(16) i32 mDepth = 0;
};

class MaterialRuntimeUI : public MaterialRuntimeDefault
{
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;
    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const override;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const override;
};