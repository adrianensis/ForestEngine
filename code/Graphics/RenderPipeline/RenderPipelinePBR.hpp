#pragma once

#include "Graphics/RenderPipeline/RenderPipeline.hpp"

class RenderPipelinePBR: public RenderPipeline
{
public:
    virtual void init() override;
    virtual void render(RenderPipelineData& renderData) override;
};
REGISTER_CLASS(RenderPipelinePBR);
