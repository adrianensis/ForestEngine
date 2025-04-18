#pragma once

#include "Core/Minimal.hpp"
#include "GPU/InstanceRenderer/GPUInstanceRenderer.hpp"
#include "GPU/Framebuffer/GPUFramebuffer.hpp"
#include "GPU/RenderPass/GPURenderPass.h"

class RenderPass;


class RenderPass : public GPURenderPass
{
public:
    virtual ~RenderPass() = default;
protected:
    virtual void updateGlobalData() override;
    virtual Matrix4 calculateProjectionViewMatrix() const override;
};
REGISTER_CLASS(RenderPass);