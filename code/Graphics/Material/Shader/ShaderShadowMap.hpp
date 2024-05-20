#pragma once

#include "Graphics/Material/Shader/ShaderDefault.hpp"

class ShaderShadowMap : public ShaderDefault
{
protected:
    virtual void fragmentShaderCode(ShaderBuilder& shaderBuilder) const override;
};
