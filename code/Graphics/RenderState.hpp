#pragma once

#include "Engine/System/System.hpp"
#include "Graphics/GPU/GPUSharedBuffer.hpp"
#include "Graphics/GPU/GPUBuffersContainer.hpp"
#include "Graphics/Material/Material.hpp"
#include "Graphics/Model/SkeletalAnimation/SkeletalAnimation.hpp"

class RenderState : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    
    void setInstanceMatrix(const Slot& slot, const Matrix4& matrix);
    Slot requestRenderInstanceSlot();
    void freeRenderInstanceSlot(Slot& slot);

private:
	std::vector<Matrix4> mMatrices;

    SlotsManager mRenderInstancesSlotsManager;
    u32 mMaxInstances = 5000;

public:
    RGET(RenderInstancesSlotsManager)
};
REGISTER_CLASS(RenderState)
