#pragma once

#include "Core/Minimal.hpp"
#include "Graphics/Light/Light.hpp"
#include "Graphics/RenderPipeline/RenderPass/RenderPass.hpp"
#include "GPU/GPUInstanceRenderer/GPUInstanceRenderer.hpp"
#include "Graphics/Renderer/MeshRenderer.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Graphics/RenderPipeline/MeshRendererManager.hpp"

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

        if(!mGPUInstanceRendererDataByRenderPass.contains(renderPassClassId))
        {
            mGPUInstanceRendererDataByRenderPass.insert({renderPassClassId, {}});
        }

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
    void compileShader(TComponentPtr<MeshRenderer> renderer);

protected:
    std::unordered_map<ClassId, OwnerPtr<RenderPass>> mRenderPassMap;
    
    // TODO: move these 2 maps to it's own container: ???
    // So it can be passed to RenderGraph and other places...
    std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUInstanceRenderer>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUInstanceRendereresMap;
	std::unordered_map<GPUInstanceRendererData, OwnerPtr<GPUShaderPipeline>, GPUInstanceRendererData::GPUInstanceRendererDataFunctor> mGPUShaderPipelines;
	std::unordered_map<ClassId, std::unordered_set<GPUInstanceRendererData, GPUInstanceRendererData::GPUInstanceRendererDataFunctor>> mGPUInstanceRendererDataByRenderPass;

    MeshRendererManager mMeshRendererManager;

public:
    CRGET(GPUInstanceRendereresMap)
    CRGET(GPUShaderPipelines)
};
REGISTER_CLASS(RenderPipeline);
