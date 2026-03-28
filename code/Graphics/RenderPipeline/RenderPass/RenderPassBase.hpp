#pragma once

#include "Core/Memory/Pointers.hpp"
#include "Engine/Core.hpp"
#include "GPU/RenderPass/GPURenderPass.h"
#include "Graphics/Camera/CameraManager.hpp"

class RenderPass;
class RenderPassBase : public GPURenderPass
{
public:
    virtual ~RenderPassBase() = default;

protected:
    virtual void updateGlobalData() override;
    virtual Maths::Matrix4 calculateProjectionViewMatrix() const override;

protected:
    Core::WeakPtr<CameraManager> mCameraManager;

public:
    SET(CameraManager)
};
REGISTER_CLASS(RenderPassBase);