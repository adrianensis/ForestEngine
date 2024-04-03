#pragma once

#include "Graphics/Material/MaterialRuntimeDefault.hpp"
#include "Graphics/Material/Material.hpp"

class MaterialPropertiesBlockUI
{
public:
    MaterialLightingModelPhong mMaterialLightingModelPhong;
    alignas(16) Vector2 mTextureRegionLeftTop = Vector2(0.0, 0.0);
    Vector2 mTextureRegionSize = Vector2(1.0, 1.0);
    alignas(16) i32 mDepth = 0;
};

class MaterialRuntimeUI : public MaterialRuntimePhong
{
protected:
    virtual std::vector<GPUStructDefinition::GPUStructVariable> generateMaterialPropertiesBlock() override;

    virtual void vertexShaderCalculateTextureCoordinateOutput(ShaderBuilder& shaderBuilder) const override;
    virtual void vertexShaderCalculatePositionOutputCustom(ShaderBuilder& shaderBuilder) const override;
};