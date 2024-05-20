#pragma once

#include "Graphics/Material/MaterialRuntime/MaterialRuntimeDefault.hpp"

class MaterialRuntimeShadowMap : public MaterialRuntimeDefault
{
protected:
    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;
};
