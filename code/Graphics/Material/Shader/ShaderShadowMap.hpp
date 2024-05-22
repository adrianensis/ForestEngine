#pragma once

#include "Graphics/Material/Shader/ShaderPBR.hpp"

class ShaderShadowMap : public ShaderPBR
{
protected:
    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;
};
