#pragma once

#include "Graphics/Shader/ShaderPBR.hpp"

class ShaderDepthBuffer : public ShaderPBR
{
protected:
    virtual void fragmentShaderCode(GPUShaderBuilder& GPUShaderBuilder) const override;
};
REGISTER_CLASS(ShaderDepthBuffer)