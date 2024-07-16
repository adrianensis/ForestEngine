#pragma once

#include "Graphics/Material/Shader/ShaderPBR.hpp"

class ShaderDepthBuffer : public ShaderPBR
{
protected:
    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;
};
REGISTER_CLASS(ShaderDepthBuffer)