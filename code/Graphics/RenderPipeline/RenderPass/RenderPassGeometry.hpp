#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPassGeometry: public RenderPass
{
public:
    virtual void render() override;
};
REGISTER_CLASS(RenderPassGeometry);
