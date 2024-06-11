#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "SpacePartition/OcTree.hpp"
#include "Graphics/RenderPipeline/RenderPipelinePBR.hpp"

class RenderEngine : public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    virtual void update() override;
    void preSceneChanged();
    void postSceneChanged();
    void onResize(u32 width, u32 height);
    
    void addComponent(Ptr<SystemComponent> component) override;
    void removeComponent(Ptr<SystemComponent> component) override;

private:
    void initBuffers();
    void swap();
    void render();
private:
	std::unordered_set<Ptr<MeshRenderer>> mRenderersSet;
    RenderPipelineData mRenderPipelineData;
    RenderPipelinePBR mRenderPipeline;

    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
    u32 mMaxInstances = 5000;

public:
	OcTree octree;
};
REGISTER_CLASS(RenderEngine);
