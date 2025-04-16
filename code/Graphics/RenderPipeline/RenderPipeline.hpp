#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "GPU/GPUInstanceRenderer/GPUInstanceRenderer.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"

class RenderPipelineData
{
public:
    std::vector<TComponentPtr<PointLight>> mPointLights;
    TComponentPtr<DirectionalLight> mDirectionalLight;
};

class RenderPipeline
{
public:
    virtual void init();
    virtual ~RenderPipeline() = default;
    void update();
    virtual void terminate();
    void addRenderer(TComponentPtr<MeshRenderer> renderer);
    void removeRenderer(TComponentPtr<MeshRenderer> renderer);
    virtual void render(RenderPipelineData& renderData);
    virtual void compile();
    void onResize();

protected:
    void updateLights(RenderPipelineData& renderData);

    template<class T> T_EXTENDS(T, RenderPass)
    void initRenderPass(const RenderPassData& renderPassData)
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        mRenderPassMap.insert_or_assign(
            renderPassClassId,
            OwnerPtr<RenderPass>::moveCast(OwnerPtr<T>::newObject())
        );

        WeakPtr<T> renderPass = getRenderPass<T>();
        renderPass->init(this, renderPassData);
    }

    template<class T> T_EXTENDS(T, RenderPass)
    WeakPtr<T> getRenderPass()
    {
        ClassId renderPassClassId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        return WeakPtr<T>::cast(mRenderPassMap.at(renderPassClassId));
    }

    void initBuffers();
    void setRendererMatrix(TComponentPtr<MeshRenderer> renderer);
    void processRenderer(TComponentPtr<MeshRenderer> renderer);
    void compileShader(TComponentPtr<MeshRenderer> renderer);

protected:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
    
    // TODO: move these 2 maps to it's own container: ???
    // So it can be passed to RenderGraph and other places...
    std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUInstanceRenderer>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUInstanceRendereresMap;
	std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;

    // TODO: move to a Renderers Container (??)
    // So it can be passed as parameter...
    std::vector<Matrix4> mMatrices;
    SlotsManager mRenderInstancesSlotsManager;
	std::vector<TComponentPtr<MeshRenderer>> mRenderers;
    std::set<u32> mUsedSlots;
	std::vector<TComponentPtr<MeshRenderer>> mRenderersStatic;
    inline static const u32 mInitialInstances = 1000;

public:
    CRGET(GPUInstanceRendereresMap)
    CRGET(GPUShaderPipelines)
};
REGISTER_CLASS(RenderPipeline);
