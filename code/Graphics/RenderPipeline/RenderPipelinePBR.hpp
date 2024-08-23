#pragma once

#include "Graphics/RenderPipeline/RenderPipeline.hpp"

class RenderPipelinePBR: public RenderPipeline
{
public:
    virtual void render(RenderPipelineData& renderData) override;
    virtual void compile() override;
};
REGISTER_CLASS(RenderPipelinePBR);
