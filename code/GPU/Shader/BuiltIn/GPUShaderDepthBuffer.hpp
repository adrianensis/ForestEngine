#pragma once

#include "GPU/Shader/BuiltIn/GPUShaderPBR.hpp"

class GPUShaderDepthBuffer : public GPUShaderPBR
{
protected:
    virtual void fragmentGPUShaderCode(GPUShaderBuilder& GPUShaderBuilder) const override;
};
REGISTER_CLASS(GPUShaderDepthBuffer)