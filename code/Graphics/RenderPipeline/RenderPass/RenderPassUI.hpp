#pragma once

#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"

class RenderPassUI: public RenderPass
{
public:
    virtual void render() override;
};
REGISTER_CLASS(RenderPassUI);

class RenderPassUIStencil: public RenderPass
{
public:
    virtual void render() override;
private:
    void renderStencilMask(ObjectId maskObjectId);
private:
    std::unordered_set<ObjectId> mMasksDrawn;
};
REGISTER_CLASS(RenderPassUIStencil);
