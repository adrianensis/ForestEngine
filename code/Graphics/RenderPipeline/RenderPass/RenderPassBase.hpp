#pragma once

#include "Engine/Core.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"

class RenderPass;
class RenderPassBase : public GPURenderPass
{
public:
    virtual ~RenderPassBase() = default;
protected:
    virtual void updateGlobalData() override;
    virtual Maths::Matrix4 calculateProjectionViewMatrix() const override;
};
REGISTER_CLASS(RenderPassBase);