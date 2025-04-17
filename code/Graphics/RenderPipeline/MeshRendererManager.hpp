#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"

class MeshRendererManager
{
public:
    virtual void init();
    void update();
    virtual void terminate();
    void addRenderer(TComponentPtr<MeshRenderer> renderer);
    void removeRenderer(TComponentPtr<MeshRenderer> renderer);

    u32 getSize() { return mRenderInstancesSlotsManager.getSize(); }

private:
    void setRendererMatrix(TComponentPtr<MeshRenderer> renderer);
    void processRenderer(TComponentPtr<MeshRenderer> renderer);

private:
    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
	std::vector<TComponentPtr<MeshRenderer>> mRenderers;
    std::set<u32> mUsedSlots;
	std::vector<TComponentPtr<MeshRenderer>> mRenderersStatic;
    inline static const u32 mInitialInstances = 1000;

public:
    CRGET(Matrices)
};
REGISTER_CLASS(MeshRendererManager);
